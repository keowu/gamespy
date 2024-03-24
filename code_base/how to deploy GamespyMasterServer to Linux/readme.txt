¯\_(ツ)_/¯ 
Ainda não tem o tutorial para a instalação do dot net runtime 8.0 mas não tem muito segredo não. roda aí o binário. vai reclamar. acessa o site baixa extrai e define as envs.

- Compile no Windows usando o recurso de publish do dotnet. e obtenha os arquivos para executar no linux.

Defina as envs para o Dotnet runtime:
export DOTNET_ROOT=/home/ubuntu/gamespymasterserver/dotnet8.2
export PATH=$PATH:/home/ubuntu/gamespymasterserver/dotnet8.2

Verifique se as portas foram liberadas no firewall:
sudo firewall-cmd --list-ports

Caso não estejam liberadas, faça:
sudo firewall-cmd --add-port=28900/tcp
sudo firewall-cmd --add-port=28900/udp

Rode o binário em background e já era:
./GameSpyMasterServer &


[!] Libere também no firewall do vendor.