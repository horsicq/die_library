Disable device driver signing in Windows
=====

Before setting BCDEdit options you might need to disable or suspend **BitLocker** and **Secure Boot** on the computer.

To disable device driver signing:

- **bcdedit /set nointegritychecks ON** 
- **bcdedit /set loadoptions DISABLE_INTEGRITY_CHECKS** 
- **bcdedit /set TESTSIGNING ON**

To enable device driver signing:

- **bcdedit /set nointegritychecks OFF**
- **bcdedit /set loadoptions DISABLE_INTEGRITY_CHECKS**
- **bcdedit /set TESTSIGNING OFF**

