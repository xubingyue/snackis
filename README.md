![logo](images/logo.png?raw=true)

# Snackis
#### secure, distributed co-creation

### Introduction
Snackis is aiming for something that might be described as a post-modern enigma-device; an effective tool that covers most secure communication needs using any regular email-account as transport. Peers, encryption-keys, messages and and settings are stored locally, encrypted using a master password. At present; Snackis supports key-exchange via invites/accepts, encryption/decryption of files, threaded feeds/posts and shared projects/tasks.

### Dependencies
Snackis requires a ```C++1z```-capable compiler and standard library to build, and defaults to using [clang](http://releases.llvm.org/download.html#4.0.0) with ```libc++```. This unfortunately often means downloading and manually installing [clang](http://releases.llvm.org/download.html#4.0.0) to even run the application, but will improve over time. Snackis further depends on ```libcurl```, ```libpthread```, ```libsodium``` and ```libuuid``` for core functionality, as well as ```GTK+ 3``` for the UI.

```
tar -xzf clang+llvm-4.0.0-x86_64-linux-gnu-ubuntu-16.04.tar.xz
cd clang+llvm-4.0.0-x86_64-linux-gnu-ubuntu-16.04
sudo cp -R * /usr/local/
sudo ldconfig
```

### Getting started
If you're running ```Linux/64```, copy [/dist](https://github.com/andreas-gone-wild/snackis/tree/master/dist) to where you want to keep Snackis; otherwise you'll have to build the executable yourself. Snackis is designed to run completely self-contained and will create everything it needs in the directory where it's started.

### Building
Once a modern enough compiler is in place, execute the following commands to build Snackis:

```
sudo apt-get install cmake libcurl4-openssl-dev libsodium-dev libuuid1 libgtk-3-dev

git clone https://github.com/andreas-gone-wild/snackis.git
mkdir snackis/build
cd snackis/build
cmake ..
make snackis
```

### Master Password
Each Snackis installation is protected by a master password that is used to encrypt the database. When started with a fresh database, the first thing Snackis will do is ask for a new password.

![login example](images/login.png?raw=true)

### UI
Snackis UI is divided into three parts; to the left is the read-only console, where output is printed; below is the reader, where commands may be given; and to the right is a stack of open views. Despite it's graphical appearance, Snackis is designed from the ground up to be effectively keyboard-driven. Each new view is pushed onto the stack when opened and popped when closed. Many buttons have mnemonics that may be revealed by pressing ```Alt```. Pressing ```Escape``` moves focus between current view and reader, and ```Ctrl-Tab``` switches between open views.

#### Style
The entire UI may be styled by editing ```/gui.css```, running the application with an empty file displays the default GTK-style.

### Setup
Once inside, the first thing you probably want to do is to have a look at the setup. Type ```setup``` and press ```Return``` in the reader to open the setup view. Imap/Smtp are required to do anything beyond local/personal use.

![setup example](images/setup.png?raw=true)

### Locking
Type ```lock``` and press ```Return``` in the reader to lock Snackis and return to the login screen, the application will keep running in the background until closed.

### Undo
Snackis logs all committed changes to an undo-stack. Type ```undo``` and press ```Return``` in the reader to manage recent changes.

![undo example](images/undo.png?raw=true)

### Encryption
Snackis supports encrypting arbitrary data for specific peers. Type ```encrypt``` and press ```Return``` in the reader to open the encryption view. On top of being encrypted; the resulting file is cryptographically signed, and the signature is verified when decrypting. Encoding ensures that the resulting file can be reliable transferred over text-based protocols, it also rougly doubles the size of the output.

![encrypt example](images/encrypt.png?raw=true)

### Decryption
Type ```decrypt``` and press ```Return``` in the reader to open the decryption view. Decoding is mandatory for files that were encoded as part of the encryption process.

![decrypt example](images/decrypt.png?raw=true)

### Invites
Type ```invite foo@bar.com``` in the reader and press ```Return``` to create a new invite. Snackis will send an email containing your public encryption-key to the specified address. When an invite is accepted, a reply is sent to inform the inviting peer. The easiest way to see how it works is to send an invite to your own address.

### Peers
Type ```peer-search``` in the reader and press ```Return``` to search Peers locally. Peers are sorted by name. A short-cut is provided to find peers by id; execute ```peer id``` to open the specified peer, any unique part of the id will do.

![peer search example](images/peer_search.png?raw=true)

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
Incoming messages arrive in the inbox, type ```inbox``` end press ```Return``` in the reader to open the inbox form.

![inbox example](images/inbox.png?raw=true)

### Feeds
Snackis main mode of communication is using feeds/posts. Type ```feed-new``` in the reader and press ```Return``` to start a new feed.

![feed example](images/feed.png?raw=true)

#### Searching
Type ```feed-search``` in the reader and press ```Return``` to search feeds. A short-cut is provided to find feeds by id, execute ```feed id``` to open the specified feed, any unique part of the id will do.

![feed search example](images/feed_search.png?raw=true)

### Posts
Type ```post-new``` in the reader and press ```Return``` to post in existing feed. The same view is used for showing existing posts. ```New Post``` opens a new post in the same feed, while ```New Reply``` opens a new post in a separate feed dedicated to this specific post.

![post example](images/post.png?raw=true)

#### Searching
Type ```post-search``` in the reader and press ```Return``` to search posts. By default, a list of last weeks posts in reverse chronological order is displayed. A short-cut is provided to find posts by id, execute ```post id``` to open the specified post, any unique part of the id will do.

![post search example](images/post_search.png?raw=true)

### Projects
Projects are sets of tasks that may be shared among a set of peers. Each project has its own feed that members may post to. Type ```project-new``` in the reader and press ```Return``` to create a new project.

![project example](images/project.png?raw=true)

#### Searching
Type ```project-search``` in the reader and press ```Return``` to search projects. Projects are sorted by name. A short-cut is provided to find projects by id, execute ```project id``` to open the specified project, any unique part of the id will do.

![project search example](images/project_search.png?raw=true)

### Tasks
Each Task belongs to a project and changes are automatically propagated to all project members. Like projects, tasks have their own feeds. Type ```task-new``` in the reader and press ```Return``` to create a new task. A short-cut is provided to find tasks by id, execute ```task id``` to open the specified task, any unique part of the id will do.

![task example](images/task.png?raw=true)

#### Searching
Type ```task-search``` in the reader and press ```Return``` to search tasks. Tasks are sorted by priority and time of creation.

![task search example](images/task_search.png?raw=true)

### Todo
Type ```todo``` in the reader and press ```Return``` to show a list of the current top-priority tasks sorted by priority and time of creation. The list includes tasks that are tagged with ```todo```. Finished tasks remain in the list for a specified time period.

![todo example](images/todo.png?raw=true)

### Algorithms
Snackis delegates anything concerning encryption to [libsodium](https://github.com/jedisct1/libsodium). The IETF-variant of ```ChaCha20-Poly1305``` is used to encrypt the master password and database, while ```XSalsa20```/```X25519``` with ```Poly1305 MAC```-authentication is used for everything else. The master password is hashed using ```Scrypt```, and encrypted using the hash as key for future validation.

### License
Snackis is licensed under the GNU General Public License Version 3.

### Help
[This is your chance](https://www.paypal.me/c4life) to help Snackis move forward, every contribution counts.<br/>