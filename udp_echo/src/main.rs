use std::net::{UdpSocket, SocketAddr, IpAddr, Ipv4Addr};
//use std::io::Error;
use std::process;
use std::env;

//use echo_sketches::parse_cli_args_fn::parse_cli_args;

fn main() {
    let argv : Vec<String> = env::args().collect();

    let arg_count = argv.len();
    println!("provided {} args", arg_count);

    for args in &argv {
        println!("comand line args is {}", &args);
    }

    //let addr : String = parse_cli_args(argv);

    let udp_socket = socket_func_wrap();

    //let connect_res = connect_func_wrap(&udp_socket, addr);

    // if connect_res.is_err() {
    //     eprintln!("connection func failed");
    //     process::exit(1);
    // } else {
    //     let addr = udp_socket.local_addr().unwrap();
    //     println!("binded with UDP address {}", addr);
    // }

    //let send_result = send_dgram(&udp_socket);

    //let mut fromAddr = SocketAddrV4::new(Ipv4Addr::new(127, 0, 0, 1), 11500);

    //let mut fromAddr = SocketAddr::new(IpAddr::V4(Ipv4Addr::new(127, 0, 0, 1)), 8080);

    //let recive_from_res = Option<usize, SocketAddr> = Ok;

    //while let Result::<(usize, std::net::SocketAddr)>.is_ok() = try_recv_from(&udp_socket, &mut fromAddr) {
    //while let recive_from_res = try_recv_from(&udp_socket, &mut fromAddr) {

    //while let Some(usize, SocketAddr::V4) = try_recv_from(&udp_socket, &mut fromAddr) {
    //while let Result::<(usize, SocketAddr)>.is_ok() = try_recv_from(&udp_socket, &mut fromAddr) {
    //    try_send_to(&udp_socket, &fromAddr);
    //}


    loop {
       let addr = try_recv_from(&udp_socket);

       try_send_to(&udp_socket, addr);
    }



    //         let _ = send_dgram(&udp_socket);
    //     } else {
    //         eprintln!("Recieve func error");
    //     }
    // }


    // if send_result.is_ok() {
    //     let bytes_sended = send_result.unwrap();
    //     println!("sended {} bytes", bytes_sended);

    // } else {
    //     eprintln!("send func mistake");
    // }

    //if send_result.is_ok() {
    //if try_recv(&udp_socket).is_ok
   // }
}

fn socket_func_wrap() -> UdpSocket {

    let initial_zero_addr = "127.0.0.1:11500";
    let udp_socket_res = UdpSocket::bind(initial_zero_addr);

    if udp_socket_res.is_err() {
        eprintln!("SOCKET func creation mistake");
        eprintln!("UNABLE create udp socket");
        process::exit(1);
    }

    udp_socket_res.unwrap()
}

// fn connect_func_wrap(udp_sock : &UdpSocket, addr_port : String) -> Result<(), Error> {
//     let addr_port_str = addr_port.as_str();
//     let connect_result =  udp_sock.connect(&addr_port_str);
//     connect_result
// }

// fn send_dgram(udp_sock : &UdpSocket) -> Result<usize, Error> {

//     let d_gram : [u8; 7] = [0, 1, 2, 3, 4, 5, 6];
//     let res = udp_sock.send(&d_gram);

//     res
// }

// fn try_recv(udp_sock : &UdpSocket) -> Result<usize, Error>  {

//     let mut buf: [u8; 7] = [0; 7];

//     let recv_res = udp_sock.recv(&mut buf);

//     if recv_res.is_ok() {
//         //let bytes_recieved = recv_res.unwrap();
//         //println!("received {bytes_recieved} bytes {:?}", &buf[..bytes_recieved]);
//         println!{"we recieved somethink"}
//     } else {
//         eprintln!("recv func mistake");
//     }

//     recv_res
// }

fn try_send_to(udp_sock : &UdpSocket, addr : SocketAddr) {

    let d_gram : [u8; 7] = [0, 1, 2, 3, 4, 5, 6];

    let res = udp_sock.send_to(&d_gram, addr);
    if res.is_ok() {
        let bytes_sended = res.unwrap();
        println!("sended {bytes_sended} bytes ");
    }
}

fn try_recv_from(udp_sock : &UdpSocket) -> SocketAddr {

    let mut buf: [u8; 7] = [0; 7];
    let recv_from_res = udp_sock.recv_from(&mut buf);
    let mut from_addr = SocketAddr::new(IpAddr::V4(Ipv4Addr::new(127, 0, 0, 1)), 8080);

    if recv_from_res.is_ok() {

        let recv_from_tule = recv_from_res.unwrap();

        let bytes_recieved = recv_from_tule.0;
        let ip = recv_from_tule.1.ip();
        let port = recv_from_tule.1.port();

        from_addr.set_ip(ip);
        from_addr.set_port(port);
        println!("received {} bytes from ip = {}, port = {}", bytes_recieved, ip, port);
    } else {
        eprintln!("recv func mistake");
    }

    from_addr
}
