#![warn(rust_2018_idioms)]

use std::{error::Error, net::SocketAddr};

use tokio::io;
use tokio_util::codec::{BytesCodec, FramedRead, FramedWrite};
use futures::StreamExt;


#[tokio::main]
async fn main() -> Result<(), Box<dyn Error>> {
    println!("TAG V{} Client starting!", common::VERSION);

    let addr = "127.0.0.1:40000".parse::<SocketAddr>()?;

    let stdin = FramedRead::new(io::stdin(), BytesCodec::new()).map(|i|
        i.map(|bytes|
            bytes.freeze()
        )
    );
    let stdout = FramedWrite::new(io::stdout(), BytesCodec::new());

    tag::connect(&addr, stdin, stdout).await?;

    Ok(())
}

mod tag {
    use bytes::Bytes;
    use futures::{future, Sink, SinkExt, Stream, StreamExt};
    use std::{error::Error, io, net::SocketAddr};
    use tokio::net::TcpStream;
    use tokio_util::codec::{BytesCodec, FramedRead, FramedWrite};

    pub async fn connect(
        addr: &SocketAddr,
        mut stdin: impl Stream<Item = Result<Bytes, io::Error>> + Unpin,
        mut stdout: impl Sink<Bytes, Error = io::Error> + Unpin,
    ) -> Result<(), Box<dyn Error>> {
        let mut stream = TcpStream::connect(addr).await?;
        let (r, w) = stream.split();
        let mut sink = FramedWrite::new(w, BytesCodec::new());
        let mut stream = FramedRead::new(r, BytesCodec::new()).filter_map(|i|
            match i {
                Ok(i) => future::ready(Some(i.freeze())),
                Err(e) => {
                    println!("Socket read error: {}", e);
                    future::ready(None)
                }
            }
        ).map(Ok);

        match future::join(sink.send_all(&mut stdin), stdout.send_all(&mut stream)).await {
            (Err(e), _) | (_, Err(e)) => Err(e.into()),
            _ => Ok(()),
        }
    }
}