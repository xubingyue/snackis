![logo](images/logo.png?raw=true)

# Snackis
#### sugar-free co-creation

### Introduction
Snackis is aiming for something that might be described as a post-modern enigma-device; a simple and convenient one-stop shop for most secure communication needs. It provides a curses-based UI on top of [libsodium](https://github.com/jedisct1/libsodium) and supports using any regular email-account as transport. Peers, encryption keys, messages and and settings are stored locally, encrypted using a master password. At present, Snackis supports key-exchange via invites/accepts, encryption/decryption of arbitrary data for a specific peer and feeds/posts. Coming up next is signed receipts, system-wide search, scheduled events.

![logo](images/gui.png?raw=true)

### Dependencies
Snackis requires a ```C++1z```-capable compiler and standard library to build, and defaults to using [clang](http://releases.llvm.org/download.html#4.0.0) with ```libc++```; it further depends on ```libcurl```, ```libpthread```, ```libsodium``` and ```libuuid``` for core functionality, as well as ```GTK+ 3``` for the UI.

```
tar -xzf clang+llvm-4.0.0-x86_64-linux-gnu-ubuntu-16.04.tar.xz
cd clang+llvm-4.0.0-x86_64-linux-gnu-ubuntu-16.04
sudo cp -R * /usr/local/

sudo apt-get install libcurl4-openssl-dev libsodium-dev libuuid1 libgtk-3-dev
```

```
git clone https://github.com/andreas-gone-wild/snackis.git
mkdir snackis/build
cd snackis/build
cmake ..
make
```

### Getting started
If you're running ```Linux/64```, copy [/dist](https://github.com/andreas-gone-wild/snackis/tree/master/dist) to where you want to keep Snackis; otherwise you'll have to build it yourself. Snackis is designed to run completely self-contained and will create everything it needs in the directory where it's started.

#### Master Password
Each snackis installation is protected by a master password that is used to encrypt the database; pick a good one, because changing it means re-encrypting the entire database.

![login example](images/login.png?raw=true)

#### Setup
Once inside, the first thing you probably want to do is to have a look at the setup. Type ```setup``` and press ```Return``` to open the settings form. Imap/Smtp-settings are currently required to do anything beyond encrypting/decrypting data for personal use.

![settings example](images/settings.png?raw=true)

#### Encryption
Snackis supports encrypting arbitrary data for a specific peer. Type ```encrypt``` and press ```Return``` in the reader to open the encryption form. On top of being encrypted; the resulting file is cryptographically signed, and the signature is verified when decrypting. Encoding ensures that the resulting file can be reliable transferred over text-based protocols, it also rougly doubles the size of the output.

![encrypt example](images/encrypt.png?raw=true)

#### Decryption
Type ```decrypt``` and press ```Return``` in the reader to open the decryption form. Decoding is mandatory for files that were encoded as part of the encryption process.

![decrypt example](images/decrypt.png?raw=true)

#### Sending Invites
Type ```invite foo@bar.com``` in the reader and press ```Return``` to open the invite form. Snackis will send an email containing your public key to the specified address. The easiest way to see how it works is to try with your own address.

![invite example](images/invite.png?raw=true)

#### Messages
All Snackis-messages look more or less the same to the outside world. Most of the actual data is appended in encrypted (except for invites) form below the ```__SNACKIS__```-marker.

![message example](images/message.png?raw=true)

##### Sending
Snackis stores all outgoing messages in an outbox that may be emptied manually at any time by typing ```send``` and pressing ```Return``` in the reader. Automatic sending at regular intervals is supported through ```Smtp Poll``` in ```Setup```.

![send example](images/send.png?raw=true)

##### Fetching
Type ```fetch``` and press return in the reader to fetch email manually. Automatic fetching at regular intervals is supported through ```Imap Poll``` in ```Setup```.

#### The Inbox
Incoming traffic that needs approval arrives in the inbox, type ```inbox``` end press ```Return``` in the reader to open the inbox form.

![inbox example](images/inbox.png?raw=true)

#### Peers
Whenever an invite is accepted/rejected, a message is sent to inform the inviting peer. Peers are uniquely identified by their email address and may be invited any number of times, encryption-keys are exchanged each time an invitation is accepted.

![accept example](images/accept.png?raw=true)

#### Feeds
Type ```new-feed``` in the reader and press ```Return``` to start a new feed. Local posts are sent to all specified peers.

![feed example](images/feed.png?raw=true)

##### Posts
Type ```post``` in the reader and press ```Return``` to post content to an existing feed.

![post example](images/post.png?raw=true)

### Algorithms
Snackis delegates anything concerning encryption to [libsodium](https://github.com/jedisct1/libsodium). The IETF-variant of ```ChaCha20-Poly1305``` is used to encrypt the master password and database, while ```XSalsa20``` with ```X25519``` key-exchange and ```Poly2305 MAC```-authentication is used for everything else. The master password is hashed using ```Scrypt```, and the password is then encrypted using the hash as key for future validation.

### License
Snackis is licensed under the GNU General Public License Version 3.

### Help
[This is your chance](https://www.paypal.me/c4life) to help Snackis move forward faster, every contribution counts. Thank you!