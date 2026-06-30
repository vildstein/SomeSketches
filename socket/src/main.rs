use std::net::UdpSocket;
//use std::net::SocketAddr;


//192.168.2.93
fn main() -> std::io::Result<()> {

    //let addr: &str = "192.168.2.93";
    //let port: &str = "6000";

    let udp_socket = UdpSocket::bind("192.168.2.93:6000").expect("Jopa jopa");


    
    
    let mut buf = [0; 10];
    let (number_of_bytes, src_addr) = udp_socket.recv_from(buf).expect("Data goes wrong");    
    
    Ok(())
}
