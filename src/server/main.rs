#![warn(rust_2018_idioms)]

// use common::sharedfn;

use tokio::io::{AsyncReadExt, AsyncWriteExt};
use tokio::net::TcpListener;

use std::error::Error;

#[tokio::main]
async fn main() -> Result<(), Box<dyn Error>> {
    println!("TAG v{} Server starting...", common::VERSION);

    let addr = "127.0.0.1:40000";

    let listener = TcpListener::bind(&addr).await?;
    println!("Listening on: {}", addr);

    loop {
        println!("Waiting for a client to conncet");
        let (mut socket, _) = listener.accept().await?;
        println!("a client connected!");

        tokio::spawn(async move {
            let mut buf = vec![0; 1024];

            loop {
                let n = socket.read(&mut buf).await.expect("Failed to read from scoket");

                if n == 0 {
                    println!("Socket had zero n (num bytes?)");
                    return;
                }

                socket.write_all(&buf[0..n]).await.expect("Failed to write to socket");
            }
        });
    }
}