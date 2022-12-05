#![warn(rust_2018_idioms)]

pub const VERSION: &str = env!("CARGO_PKG_VERSION");

pub fn sharedfn() {
    println!("Just doing common things");
}

pub mod network {
    // pub const DEFAULT_SERVER_IP = "127.0.0.1";
    // pub const DEFAULT_SERVER_PORT = "40000";
    pub const MAX_DATAGRAM_SIZE: usize = 65_507;

    pub enum Message {
        RequestToJoin,
        ServerMessage(String),
        SetNickName(String),
        ChatMessage(String),
    }
}