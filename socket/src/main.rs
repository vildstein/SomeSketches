use std::net::UdpSocket;

fn main() -> std::io::Result<()> {

    let udp_socket = UdpSocket::bind("192.168.2.93:6000").expect("connction expected");    
    
    let mut buf: [u8; 7] = [0;7];

    modyfy_array(&mut buf);

    udp_socket.send(&buf).expect("really !?");        
    
    Ok(())
}


fn modyfy_array(arr : &mut [u8;7]) -> () {

    arr[0] = 0x0FF;
    arr[1] = 0x001;
    arr[2] = 0x002;
    arr[3] = 0x003;
    arr[4] = 0x004;
    arr[5] = 0x005;
    arr[6] = 0x006;
}