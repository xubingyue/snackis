![logo](images/logo.png?raw=true)

# Snackis
#### distributed & secure co-creation

### Introduction
Snackis is aiming for something that might be described as a post-modern enigma-device; a tool that covers most secure communication needs using any regular email-account as transport. Peers, encryption keys, messages and and settings are stored locally, encrypted using a master password. At present; Snackis supports key-exchange via invites/accepts, encryption/decryption of arbitrary data for specific peers and feeds/posts. Coming up next is signed receipts, scheduled events and more.

### Dependencies
Snackis requires a ```C++1z```-capable compiler and standard library to build, and defaults to using [clang](http://releases.llvm.org/download.html#4.0.0) with ```libc++```. This unfortunately often means downloading and manually installing [clang](http://releases.llvm.org/download.html#4.0.0) to even run the application, but will improve over time. Snackis further depends on ```libcurl```, ```libpthread```, ```libsodium``` and ```libuuid``` for core functionality, as well as ```GTK+ 3``` for the UI.

```
tar -xzf clang+llvm-4.0.0-x86_64-linux-gnu-ubuntu-16.04.tar.xz
cd clang+llvm-4.0.0-x86_64-linux-gnu-ubuntu-16.04
sudo cp -R * /usr/local/

sudo apt-get install libcurl4-openssl-dev libsodium-dev libuuid1 libgtk-3-dev
```

### Getting started
If you're running ```Linux/64```, copy [/dist](https://github.com/andreas-gone-wild/snackis/tree/master/dist) to where you want to keep Snackis; otherwise you'll have to build the ```snackis```-executable yourself. Snackis is designed to run completely self-contained and will create everything it needs in the directory where it's started.

### Building
Once all dependencies are in place, execute the following commands to build Snackis:

```
git clone https://github.com/andreas-gone-wild/snackis.git
mkdir snackis/build
cd snackis/build
cmake ..
make snackis
```

### Master Password
Each snackis installation is protected by a master password that is used to encrypt the database; pick a good one, because changing it means re-encrypting the entire database.

![login example](images/login.png?raw=true)

### UI
Snackis UI is divided into three parts; to the left is the read-only console, where output is printed; below is the auto-completed reader, where commands may be given; and to the right is an optional stack of open views. Each new view is pushed onto the stack when opened and popped when closed. All buttons have mnemonics that may be revealed by pressing ```Alt```. Pressing ```Escape``` moves focus between current view and reader.

#### Style
The entire UI may be styled to your taste by editing ```gui.css``` in the root.

### Setup
Once inside, the first thing you probably want to do is to have a look at the setup. Type ```setup``` and press ```Return``` in the reader to open the setup view. Imap/Smtp are currently required to do anything beyond encrypting/decrypting data for personal use.

![setup example](images/setup.png?raw=true)

### Locking
Type ```lock``` and press ```Return``` in the reader to lock Snackis and return to the login screen, the application will keep running in the background until closed.

### Encryption
Snackis supports encrypting arbitrary data for specific peers. Type ```encrypt``` and press ```Return``` in the reader to open the encryption view. On top of being encrypted; the resulting file is cryptographically signed, and the signature is verified when decrypting. Encoding ensures that the resulting file can be reliable transferred over text-based protocols, it also rougly doubles the size of the output.

![encrypt example](images/encrypt.png?raw=true)

### Decryption
Type ```decrypt``` and press ```Return``` in the reader to open the decryption view. Decoding is mandatory for files that were encoded as part of the encryption process.

![decrypt example](images/decrypt.png?raw=true)

### Invites
Type ```invite foo@bar.com``` in the reader and press ```Return``` to create a new invite. Snackis will send an email containing your public encryption key to the specified address. When an invite is accepted/rejected, a reply is sent to inform the inviting peer. The easiest way to see how it works is to try with your own address.

### Messages
All Snackis-messages look more or less the same to the outside world, the actual data is appended in encrypted (except for invites) and encoded form below the ```__SNACKIS__```-marker.

![message example](images/message.png?raw=true)

### Sending
Snackis stores all outgoing messages in an outbox that may be manually emptied at any time by typing ```send``` and pressing ```Return``` in the reader. Automatic sending at regular intervals is supported through ```Smtp Poll``` in ```Setup```.

### Fetching
Type ```fetch``` and press return in the reader to fetch email manually. Automatic fetching at regular intervals is supported through ```Imap Poll``` in ```Setup```.

### Email Security
If you haven't previously used external clients with your email-address, it's quite likely that you'll get an error when Snackis tries connecting to the server. Most email providers will send a message to your inbox with a warning and instructions on how to enable external access. Rest assured that there is nothing insecure about Snackis way of accessing your email, and as long as you don't type in your server-details anywhere else nothing bad can happen as a consequence of allowing external access to your account.

### Inbox
Incoming traffic that needs approval arrives in the inbox, type ```inbox``` end press ```Return``` in the reader to open the inbox form.

![inbox example](images/inbox.png?raw=true)

### Feeds
Snackis main mode of communication is through feeds/posts. Type ```feed``` in the reader and press ```Return``` to start a new feed. Local posts are sent to all specified peers, and any peer may reply.

![feed example](images/feed.png?raw=true)

#### Searching
Type ```feed-search``` in the reader and press ```Return``` to search feeds.

![feed search example](images/feed_search.png?raw=true)

### Posts
Type ```post``` in the reader and press ```Return``` to post in existing feed. Existing posts may be updated and updates are propagated to all local feed peers.

![post example](images/post.png?raw=true)

#### Searching
Type ```post-search``` in the reader and press ```Return``` to search posts. By default, a list of last weeks posts in reverse chronological order is displayed.

![post search example](images/post_search.png?raw=true)

### Algorithms
Snackis delegates anything concerning encryption to [libsodium](https://github.com/jedisct1/libsodium). The IETF-variant of ```ChaCha20-Poly1305``` is used to encrypt the master password and database, while ```XSalsa20```/```X25519``` with ```Poly1305 MAC```-authentication is used for everything else. The master password is hashed using ```Scrypt```, and encrypted using the hash as key for future validation.

### License
Snackis is licensed under the GNU General Public License Version 3.

### Help
[This is your chance](https://www.paypal.me/c4life) to help Snackis move forward, every contribution counts.<br/>