use std::net::UdpSocket;
use std::net::SocketAddr;


//192.168.2.93
fn main() -> std::io::Result<()> {


    let socket_addr = SocketAddr::new(IpAddr::V4(Ipv4Addr::new(192, 168, 2, 93)), 6000);
    let udp_socket = UdpSocket::bind(socket_addr).expect("Jopa jopa");


    let mut buf = [0; 10];
    let (number_of_bytes, src_addr) = udp_socket.recv_from(buf).expect("Data goes wrong");    
    
    Ok(())
}
