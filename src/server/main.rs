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
use tokio_stream::StreamExt;
use tokio_util::codec::BytesCodec;
use tokio_util::udp::UdpFramed;

use bytes::Bytes;
use bytes::BytesMut;
use futures::{FutureExt, SinkExt};

#[tokio::main]
async fn main() -> Result<(), Box<dyn Error>> {
    println!("TAG v{} Server starting...", common::VERSION);

    let addr = "127.0.0.1:40000";
    let server = Arc::new(Mutex::new(Server::new(addr)));

    loop {
        let server = Arc::clone(&server);
        let (bytes, remote) = server.lock().await.socket.next().map(|e| e.unwrap()).await?;
        tokio::spawn(async move {
            let peer = server.lock().await.peers.entry(remote).or_insert(Peer::new());
            peer.process_packet(server.clone(), bytes, remote).await 
        });
    }
}

struct Server {
    socket: UdpFramed<BytesCodec>,
    peers: HashMap<SocketAddr, Peer>,
}

impl Server {
    async fn new(listen_on: &str) -> Result<Self, io::Error> {
        let raw_socket = UdpSocket::bind(&listen_on).await?;
        let framed_socket = UdpFramed::new(raw_socket, BytesCodec::new());
        println!("Listening on: {}", listen_on);

        Ok(Server {
            socket: framed_socket,
            peers: HashMap::new(),
        })
    }
}

#[derive(Debug, Clone)]
enum PeerState {
    New,
}

#[derive(Debug)]
struct Peer {
    state: PeerState,    
}

impl Peer {
    fn new() -> Self {
        Peer {
            state: PeerState::New,
        }
    }

    async fn process_packet(&mut self, server: Arc<Mutex<Server>>, bytes: BytesMut, remote_address: SocketAddr) -> Result<(), io::Error> {
        println!("Processing packet from {:?} in {:?} state.", &remote_address, self.state);
        match self.state {
            PeerState::New => {
                server.lock().await.socket.send((Bytes::from(bytes), remote_address));
            }
        }

        Ok(())
    }
}