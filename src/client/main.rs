#![warn(rust_2018_idioms)]

use std::error::Error;
use std::net::SocketAddr;
use std::sync::Arc;

use tokio::io;
use tokio::net::UdpSocket;
use tokio_util::codec::{BytesCodec, FramedRead, FramedWrite};

use bytes::Bytes;

use futures::{SinkExt, StreamExt};

#[tokio::main]
async fn main() -> Result<(), Box<dyn Error>> {
    println!("TAG V{} Client starting!", common::VERSION);

    let addr = "127.0.0.1:40000".parse::<SocketAddr>()?;

    let local_addr: SocketAddr = if addr.is_ipv4() {
        "0.0.0.0:0"
    } else {
        "[::]:0"
    }.parse()?;

    let socket = UdpSocket::bind(local_addr).await?;
    socket.connect(&addr).await?;
    let recieve_socket = Arc::new(socket);
    let send_socket = recieve_socket.clone();

    let sender = tokio::spawn(async move {
        let _ = send(&send_socket).await;
    });
    let reciever = tokio::spawn(async move {
        let _ = recv(&recieve_socket).await;
    });
    let res = tokio::try_join!(sender, reciever);

    if let Err(err) = res {
        println!("error: {:?}", err);
    }

    Ok(())
}

async fn send(socket: &UdpSocket) -> Result<(), io::Error> {
    let mut stdin = FramedRead::new(io::stdin(), BytesCodec::new()).map(|i|
        i.map(|bytes|
            bytes.freeze()
        )
    );

    while let Some(text) = stdin.next().await {
        let buf = text?;
        socket.send(&buf[..]).await?;
    }

    Ok(())
}

async fn recv(socket: &UdpSocket) -> Result<(), io::Error> {
    let mut stdout = FramedWrite::new(io::stdout(), BytesCodec::new());

    loop {
        let mut buf = vec![0; 1024];
        let bytes_recieved = socket.recv(&mut buf[..]).await?;

        if bytes_recieved > 0 {
            print!("> ");
            stdout.send(Bytes::from(buf)).await?;
        }
    }
}
