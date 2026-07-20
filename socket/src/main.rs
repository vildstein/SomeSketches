use std::io::Error;
use std::net::UdpSocket;
use std::env;

fn main() -> std::io::Result<()> {

    let argv : Vec<String> = env::args().collect();
    let mut addr: &str = "127.0.0.1";
    let mut port: &str = "7500";

    //let defined_addr_status  = "defined adress = {addr}, defined port = {port}";

    match argv.len() {
        2 => {
            addr = argv[1].as_str();
        },
        3 => {
            addr = argv[1].as_str();
            port = argv[2].as_str();
        },
        _ => println!("Too much args. Exit."),
    }

    println!("ip = {addr}, port = {port}");

    let udp_socket = UdpSocket::bind("127.0.0.1:7500").expect("connction expected");

    let res = connect_func_wrap(&udp_socket, addr, port);

    if res.is_err() {
        println!("connection func failed")
        // EXIT(1)
    } else {
        // Ok()
        println!("connected with UDP");
    }


    //udp_socket
    //    .connect("127.0.0.1:6000")
   //     .expect("connect function failed");

    //let buf: [u8; 7] = [0, 1, 2, 3, 4, 5, 6];

    //for el in &buf {
    //    println!("{}", el);
    // }

    //udp_socket.send(&buf).expect("really !?");

    Ok(())
}

fn connect_func_wrap(udp_sock : &UdpSocket, addr : &str, port: &str) -> Result<(), Error> {

    let mut full_addr = String::new();
    full_addr.push_str(addr);
    full_addr.push_str(":");
    full_addr.push_str(port);

    let connect_result = udp_sock.connect(&full_addr[0..]);

    connect_result
}
