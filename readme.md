**Helical: A Simple Qt Based SSH Client**
***

Helical is a Qt based SSH client for connecting to SSH servers and allowing the execution of remote commands , running of a remote shell or accessing the remote filesystem using SFTP. It supports password and public key user authorization and public key server verification through ".ssh/known_hosts".

The client is written mostly in Qt/C++ but it uses a set of Antik wrapper classes for talking to 'C' library [libssh](https://www.libssh.org/)  which provides the client side SSH support.

A remote shell session takes place inside a window that emulates a vt100 terminal (supports a subset of vt100 escape sequences) that is currently on fixed size 80x24. Output from any remotely executed commands (stdout/stderr) is sent to the session log.

The SFTP browser displays both local and remote files systems and selection of single files (at present) for upload/download/viewing.

**To Do List**
***
- Custom view/model implementaiton for vt100 to support character attributes/color.
- Customise terminal size.
- Support for more than one running session.
- SCP client.
- Use view/model for remote SFTP file system instead of QListWidget,
- Better server disconnect handling (hard as libssh is awful on that front).