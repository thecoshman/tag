#![warn(rust_2018_idioms)]

#![allow(dead_code)]
#![allow(unused_variables)]

use std::collections::HashMap;
use std::error::Error;
use std::net::SocketAddr;
use std::sync::Arc;

use tokio::net::UdpSocket;
use tokio::io;
use tokio::sync::{mpsc, Mutex};
use tokio_util::codec::BytesCodec;
use tokio_util::udp::UdpFramed;

use bytes::Bytes;
use bytes::BytesMut;
use futures::{FutureExt, SinkExt, StreamExt};

use common::network::Message;

#[tokio::main]
async fn main() -> Result<(), Box<dyn Error>> {
    println!("TAG v{} Server starting...", common::VERSION);

    let addr = "127.0.0.1:40000";

    let (mut sink, mut stream) = {
        let raw_socket = UdpSocket::bind(&addr).await?;
        let framed_socket = UdpFramed::new(raw_socket, BytesCodec::new());
        println!("Listening on: {}", addr);
        framed_socket.split()
    };
    let peers = Arc::new(Mutex::new(HashMap::new()));
    let (peer_packets_tx, mut peer_packets_rx) = mpsc::unbounded_channel();  
    // peer_packets_tx => Cloned for each peer to request packets be sent
    // perr_packets_rx => Read by the server to know when there's packets to send for a peer

    // Send any outgoing packets
    let sending_task_peers = peers.clone();
    let packet_sending_task = tokio::spawn(async move {
        println!("Packet sender task started");
        while let Some((message, address)) = peer_packets_rx.recv().await {
            println!("Sending a packet to {:?}", address);
            match message {
                Message::RequestToJoin => {}
                Message::ServerMessage(msg) => {
                    let _ = sink.send((Bytes::from(msg), address)).await;
                }
                Message::SetNickName(_) => {}
                Message::ChatMessage(msg) => {
                    let mut peers  = sending_task_peers.lock().await;
                    for peer in peers.iter_mut() {
                        if *peer.0 != address {
                            let _ = sink.send((Bytes::from(msg.clone()), *peer.0)).await;
                        }
                    }
                }
            }
        }
    });

    // Handle any incoming packets
    let packet_receving_task = tokio::spawn(async move {
        println!("Waiting for a new packet to come into server");
        while let Ok((bytes, remote)) = stream.next().map(|e| e.unwrap()).await {
            println!("Got a packet from {:?}", remote);
            // In order to keep this fast, we don't even verify packets, just find the 'peer' and give it the packet to deal with
            // Might need to do _some_ validation here if it requires server wide data
            let mut peers  = peers.lock().await;
            let peer = peers.entry(remote).or_insert({
                Peer::new(peer_packets_tx.clone())
            });
            let _ = peer.process_packet(bytes, remote).await;
        }
    });

    let _ = tokio::try_join!(packet_sending_task, packet_receving_task);

    Ok(())
}

#[derive(Debug, Clone)]
enum PeerState {
    New,
    Awaiting_Name,
    Joined,
}

#[derive(Debug)]
struct Peer {
    state: PeerState,
    packet_tx: mpsc::UnboundedSender<(Message, SocketAddr)>, // given to the peer so that it can request packets be sent
    name: String,
}

impl Peer {
    fn new(tx: mpsc::UnboundedSender<(Message, SocketAddr)>) -> Self {
        Peer {
            state: PeerState::New,
            packet_tx: tx,
            name: "".to_string(),
        }
    }

    async fn process_packet(&mut self, bytes: BytesMut, remote_address: SocketAddr) -> Result<(), io::Error> {
        println!("Processing packet from {:?} in {:?} state.", &remote_address, self.state);

        // Do basic verification to make sure this packet really is valid etc
        match self.state {
            PeerState::New => {
                let _ = self.packet_tx.send((Message::ServerMessage("Welcome, please give your name\n".to_string()), remote_address));
                self.state = PeerState::Awaiting_Name;
            }
            PeerState::Awaiting_Name => {
                self.name = String::from_utf8((&bytes).to_vec()).unwrap();
                let _ = self.packet_tx.send((Message::ServerMessage("Enjoy the chat :)\n".to_string()), remote_address));
                self.state = PeerState::Joined;
            }
            PeerState::Joined => {
                let _ = self.packet_tx.send((Message::ChatMessage(String::from_utf8((&bytes).to_vec()).unwrap()), remote_address));
            }
        }

        Ok(())
    }
}