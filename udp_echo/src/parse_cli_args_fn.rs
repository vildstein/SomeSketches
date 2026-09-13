

// TO DO
// check was correct ot incorrecct ip and port
// check empty flags
// add buff capacity as args
// add tcp or udp from flags

pub fn parse_cli_args(argv : Vec<String>) -> String {

    let argc = argv.len();
    let mut addr_port: String = String::new();

    let mut addr :&str = "127.0.0.1";
    let mut port :&str = "9000";

    addr_port.push_str(addr);
    addr_port.push(':');
    addr_port.push_str(port);

    match argc {
            2 => {
                addr = argv[1].as_str();
                addr_port = addr_port.as_str().replace("127.0.0.1", &addr);
            },
            3 => {
                addr = argv[1].as_str();
                addr_port = addr_port.as_str().replace("127.0.0.1", &addr);
                port = argv[2].as_str();
                addr_port = addr_port.as_str().replace("9000", &port);
            },
            _ => println!("no ip and port numbers was provided"),
        }

        println!("ip adress defined as = {}", addr);
        println!("destination port defined as = {}", port);

        addr_port
}
