use std::net::UdpSocket;

fn main() -> std::io::Result<()>  {
    
    let udp_socket = UdpSocket::bind("127.0.0.1:7500").expect("connction expected");
    udp_socket.connect("127.0.0.1:6000").expect("connect function failed");

    
    let buf:[u8; 7]  = [0, 1, 2, 3, 4, 5, 6];

    for el in &buf {
        println!("{}", el);
    }
    
    udp_socket.send(&buf).expect("really !?");
    
    Ok(())
}