#![warn(rust_2018_idioms)]

use std::error::Error;
use std::net::SocketAddr;

use tokio::net::UdpSocket;

#[tokio::main]
async fn main() -> Result<(), Box<dyn Error>> {
    println!("TAG v{} Server starting...", common::VERSION);

    let addr = "127.0.0.1:40000";

    let socket = UdpSocket::bind(&addr).await?;
    let mut buf: Vec<u8> = vec![0; 1024];
    let mut to_send: Option<(usize, SocketAddr)> = None;
    println!("Listening on: {}", addr);
    loop {
        if let Some((size, peer)) = to_send {
            println!("Sending a message");
            socket.send_to(&buf[..size], &peer).await?;
        }
        println!("Waiting for something to send back");
        to_send = Some(socket.recv_from(&mut buf).await?);
    }

}
