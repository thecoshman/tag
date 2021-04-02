#![warn(rust_2018_idioms)]

use std::collections::HashMap;
use std::error::Error;
use std::io;
use std::net::SocketAddr;
use std::sync::Arc;

use tokio::sync::{mpsc, Mutex};
use tokio::net::{TcpListener, TcpStream};
use tokio_util::codec::{Framed, LinesCodec};
use tokio_stream::StreamExt;

use futures::SinkExt;

#[tokio::main]
async fn main() -> Result<(), Box<dyn Error>> {
    println!("TAG v{} Server starting...", common::VERSION);

    let addr = "127.0.0.1:40000";

    let listener = TcpListener::bind(&addr).await?;
    println!("Listening on: {}", addr);

    let state = Arc::new(Mutex::new(Shared::new()));

    loop {
        println!("Waiting for a client to conncet");
        let (stream, addr) = listener.accept().await?;

        let state = Arc::clone(&state);

        tokio::spawn(async move {
            println!("a client connected!");
            if let Err(e) = process(state, stream, addr).await {
                println!("Some error... {:?}", e);
            }
        });
    }
}

type Tx = mpsc::UnboundedSender<String>;

type Rx = mpsc::UnboundedReceiver<String>;

struct Shared {
    peers: HashMap<SocketAddr, Tx>,
}

struct Peer {
    lines: Framed<TcpStream, LinesCodec>,
    rx: Rx,
}

impl Shared {
    fn new() -> Self {
        Shared {
            peers: HashMap::new(),
        }
    }

    async fn broadcast(&mut self, sender: SocketAddr, message: &str) {
        for peer in self.peers.iter_mut() {
            if *peer.0 != sender {
                let _ = peer.1.send(message.into());
            }
        }
    }
}

impl Peer {
    async fn new(
        state: Arc<Mutex<Shared>>,
        lines: Framed<TcpStream, LinesCodec>,
    ) -> io::Result<Peer> {
        let addr = lines.get_ref().peer_addr()?;
        let (tx, rx) = mpsc::unbounded_channel();
        state.lock().await.peers.insert(addr, tx);

        Ok(Peer{ lines, rx})
    }
}

async fn process (
    state: Arc<Mutex<Shared>>,
    stream: TcpStream,
    addr: SocketAddr,
) -> Result<(), Box<dyn Error>> {
    let mut lines = Framed::new(stream, LinesCodec::new());

    // Handle greating the client
    lines.send("Please enter a nickname").await?;

    let nickname = match lines.next().await {
        Some(Ok(line)) => line,
        _ => {
            println!("Client {} sent funky data for nickname. Adios pal!", addr);
            return Ok(());
        }
    };

    // client accepted
    let mut peer = Peer::new(state.clone(), lines).await?;

    // broadcast this info to all other clients
    {
        let mut state = state.lock().await;
        let msg = format!("{} has joined the chat", nickname);
        state.broadcast(addr, &msg).await;
    }

    // Whilst the client is connected
    loop {
        tokio::select! {
            Some(msg) = peer.rx.recv() => {
                peer.lines.send(&msg).await?;
            }
            result = peer.lines.next() => match result {
                // broadcast clients message to all others
                Some(Ok(msg)) => {
                    let mut state = state.lock().await;
                    let msg = format!("{}: {}", nickname, msg);
                    state.broadcast(addr, &msg).await;
                }
                // Error!
                Some(Err(e)) => println!("error handling message from {}: {:?}", nickname, e),
                // Stream exhausted
                None => break,
            }
        }
    }

    // For whatever reason, client isn't there any more
    {
        let mut state = state.lock().await;
        state.peers.remove(&addr);

        let msg = format!("{} is no more", nickname);
        state.broadcast(addr, &msg).await;
    }

    Ok(())
}
