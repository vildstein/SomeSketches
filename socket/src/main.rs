use std::io::Error;
use std::net::{UdpSocket};
use std::env;
use std::process;

fn main() -> std::io::Result<()> {

    let argv : Vec<String> = env::args().collect();
    let mut addr_port: String = String::from("127.0.0.1:7500");
    let mut addr :&str = "127.0.0.1";
    let mut port :&str = "7500";

    match argv.len() {
        2 => {
            addr = argv[1].as_str();
            addr_port = addr_port.as_str().replace("127.0.0.1", &addr)
        },
        3 => {
            addr = argv[1].as_str();
            addr_port = addr_port.as_str().replace("127.0.0.1", &addr);
            port = argv[2].as_str();
            addr_port = addr_port.as_str().replace("7500", &port);
        },
        _ => println!("Too much args."),
    }

    let udp_socket = socket_func_wrap();
    let res = connect_func_wrap(&udp_socket, &addr_port.as_str());

    if res.is_err() {
        println!("connection func failed");
        process::exit(0);
    } else {
        println!("binded with UDP");
        println!("ip = {}, port = {}", addr, port);
    }

    send_message(&udp_socket);

    Ok(())
}

fn socket_func_wrap() -> UdpSocket {

    let initial_zero_addr = "0.0.0.0:0";
    let udp_socket_res = UdpSocket::bind(initial_zero_addr);//.expect("SOCKET func creation mistake");

    if udp_socket_res.is_ok() {
        return udp_socket_res.unwrap();
    } else {
        println!("SOCKET func creation mistake");
        println!("UNABLE create udp socket");
        process::exit(0);
    }
}

fn connect_func_wrap(udp_sock : &UdpSocket, addr_port : &str) -> Result<(), Error> {

    let connect_result =  udp_sock.connect(&addr_port);
    connect_result
}

fn send_message(udp_sock : &UdpSocket) {

    let mut buf: [u8; 7] = [0; 7];

    buf[0] = 0xFF;
    buf[1] = 0x01;
    buf[2] = 0x00;
    buf[3] = 0x4b;

    let az : u32 = 18500;
    let az_msb: u32 = az >> 8;
    let az_lsb: u32 = az;

    let converted_az_msb = az_msb as u8;
    buf[4] = converted_az_msb;

    let converted_az_lsb = az_lsb as u8;
    buf[5] = converted_az_lsb;

    let check_summ: u8 = buf[1] + buf[2] + buf[3] + buf[4] + buf[5];
    buf[6] = check_summ;

    udp_sock.send(&buf).expect("couldn't send message");
}
