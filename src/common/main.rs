#![warn(rust_2018_idioms)]

pub const VERSION: &'static str = env!("CARGO_PKG_VERSION");

pub fn sharedfn() {
    println!("Just doing common things");
}