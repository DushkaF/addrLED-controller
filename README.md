Репозиторий кода управления светодиодами

Для того чтобы склонировать новый репозиторий:
1. GIT Bash here в папке, куда хочешь положить проект
2. Команда: `git clone + url репозитория`

Для апдейта:
1. GIT Bash here в папке, ult проект
2. Команды: 
```
git stash
git pull
```
Для создания бинарника SPIFFS:
  https://github.com/igrr/mkspiffs
  `.\mkspiffs.exe -c data/ -d 5 -b 8192 -p 256 -s 512000 spiffs.bin`
