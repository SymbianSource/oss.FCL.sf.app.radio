@echo Original environment restoration by settingstestmodule...
del \epoc32\winscw\c\TestFramework\SettingsTestModule.cfg
del \epoc32\wins\c\TestFramework\TestFramework.ini
del \epoc32\winscw\c\TestFramework\TestFramework.ini
copy ..\cenrep\2002FF52.txt  \epoc32\data\z\private\10202be9\2002FF52.txt
copy ..\cenrep\2002FF52.txt  \epoc32\RELEASE\winscw\UDEB\Z\private\10202be9\2002FF52.txt
copy ..\cenrep\2002FF52.txt  \epoc32\RELEASE\winscw\UREL\Z\private\10202be9\2002FF52.txt
del \epoc32\winscw\c\private\10202BE9\persists\2002FF52.cre
@echo Original environment restoration by settingstestmodule... Finished.