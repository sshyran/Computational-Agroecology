## Development Dependencies

#### Node.js

For **Linux** user,

```bash
$ sudo apt install npm
$ npm install -g npx

# node js 8.x
$ curl -sL https://deb.nodesource.com/setup_8.x | sudo -E bash -
$ sudo apt-get install -y nodejs
```

For **Windows** user, Please install [Node.js](<https://nodejs.org/en/download/>), and install `npx` by 

```powershell
$ npm install -g npx
```

#### gRPC

For **Linux** user, please follow the instruction in `../agent_server/README.md`.

For **Windows** user, please download [protoc.exe](https://github.com/protocolbuffers/protobuf/releases), and make sure it is  executable and discoverable in current path. 

#### gRPC-web

For **Linux** User,

```bash
$ git clone https://github.com/grpc/grpc-web.git
$ cd grpc-web
$ sudo make install-plugin
```

For **Windows** User, please download [protoc-gen-grpc-web.exe](<https://github.com/grpc/grpc-web/releases>) plugin, and make sure it is  executable and discoverable in current path. 

#### Version

For **Windows**, the workflow  is ensured with following version

- npm  6.4.1
- Node 8
- [Protocol Buffers v3.8.0](https://github.com/protocolbuffers/protobuf/releases/tag/v3.8.0)
- [grpc/grpc-web 1.0.4](https://github.com/grpc/grpc-web/releases/tag/1.0.4)

For **Linux**,

+ gcc/g++ 7.x
+ Node 8
+ npm 3.5.2
+ protoc 3.7.0



## Build distributed code

### Compilation

run following commands in shell:

```bash
$ npm install
$ npm run build

# for windows user instead
$ npm run build-windows
```

copy the whole `/public` subdirectory to deployment path

### Node.js based compilation and test 

After compilation, run following commands in shell:

```shell
# current testing grpc-web connectivity
$ npm run client
$ npm run start
```

open browser, enter `localhost:8000`

## Deployment Dependencies

**Assume that the site is deployed on `Ubuntu 18.04`.**

#### Nginx (Optional)

Install

```bash
$ sudo apt update
$ sudo apt install nginx
```

Update firewall config

```bash
$ sudo ufw allow 'Nginx HTTP'
$ sudo ufw status
```

#### Apache2 (Optional)

If you are using a apache2 based Ubuntu server, like us, you can do the following

```bash
# disable default website
$ sudo a2dissite *default
$ sudo bash -c "echo '<VirtualHost *:80>
  DocumentRoot /var/www/public/
  DirectoryIndex index.html
  LogLevel warn
  ErrorLog  /var/www/public/log/error.log
  CustomLog /var/www/public/log/access.log combined
</VirtualHost>
' > /etc/apache2/sites-available/CompAgGame.conf"
$ cat /etc/apache2/sites-available/CompAgGame.conf
$ sudo cp -rf public /var/www/public
$ sudo mkdir -p /var/www/public/log
$ sudo a2ensite CompAgGame.conf
$ sudo systemctl reload apache2
```



#### Docker

Reference: [How To Install and Use Docker on Ubuntu 18.04](<https://www.digitalocean.com/community/tutorials/how-to-install-and-use-docker-on-ubuntu-18-04>)

```bash
$ sudo apt update
$ sudo apt install apt-transport-https ca-certificates curl software-properties-common
curl -fsSL https://download.docker.com/linux/ubuntu/gpg | sudo apt-key add -
$ sudo add-apt-repository "deb [arch=amd64] https://download.docker.com/linux/ubuntu bionic stable"
$ sudo apt update
$ sudo apt install docker-ce

# check status
$ sudo systemctl status docker

# execute Docker without sudo (optional)
$ sudo usermod -aG docker ${USER} # $USER for current username
```

#### Envoy

Install image

```bash
$ sudo docker pull envoyproxy/envoy
```

build new image and run

```bash
$ cd envoy
# modify address to static IP of your server in ./envoy.yaml
$ docker build -t agent/envoy -f ./envoy.Dockerfile .
$ docker run -d -p 8080:8080 --network=host agent/envoy
 
# for windows or mac
$ docker run -d -p 8080:8080 agent/envoy
```

#### Backend

```bash
# to run
$ cd ../agent_server
$ ./agent_server
$ (Ctrl+z)
$ bg
$ disown

# to stop
$ kill -9 `pgrep -f agent_server`
```

