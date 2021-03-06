
#include "mainwindow.h"
#include "ui_mainwindow.h"

void MainWindow::socketHandler(int socketDescriptor, Mensagem mensagem) {
    int byteslidos;

    if (socketDescriptor == -1) {
        printf("Falha ao executar accept().");
        exit(EXIT_FAILURE);
    }

    byteslidos = recv(socketDescriptor,&mensagem,sizeof(mensagem),0);
    cout << byteslidos << endl;

    this->mensagem = mensagem;
    updateTrains();

    if (byteslidos == -1) {
        printf("Falha ao executar recv()");
        exit(EXIT_FAILURE);
    }

    else if (byteslidos == 0) {
        printf("Cliente finalizou a conexão\n");
    }
    ::close(socketDescriptor);
}

void MainWindow::server() {
    struct sockaddr_in endereco;

    /*Variáveis relacionadas com as conexões clientes*/
    struct sockaddr_in enderecoCliente;
    socklen_t tamanhoEnderecoCliente = sizeof(struct sockaddr);
    int conexaoClienteId;

    Mensagem mensagem;

    memset(&endereco, 0, sizeof(endereco));
    endereco.sin_family = AF_INET;
    endereco.sin_port = htons(PORTNUM);
    endereco.sin_addr.s_addr = inet_addr(IP_SERV);

    socketId = socket(AF_INET, SOCK_STREAM, 0);

    if (socketId == -1) {
        printf("Falha ao executar socket().\n");
        exit(EXIT_FAILURE);
    }

    if (bind(socketId, (struct sockaddr *)&endereco, sizeof(struct sockaddr)) == -1 ) {
        printf("Falha ao executar bind().\n");
        exit(EXIT_FAILURE);
    }

    if (listen( socketId, 10 ) == -1) {
        printf("Falha ao executar listen().\n");
        exit(EXIT_FAILURE);
    }

    /*Servidor ficar em um loop infinito*/
    while(true) {
        printf("Servidor: esperando conexões clientes\n");

        /*Servidor fica bloqueado esperando uma conexão do cliente*/
        conexaoClienteId = accept(socketId,(struct sockaddr *) &enderecoCliente, &tamanhoEnderecoCliente);
        printf("Servidor: recebeu conexão de %s\n", inet_ntoa(enderecoCliente.sin_addr));
        threadServer = std::thread(&MainWindow::socketHandler,this, conexaoClienteId, mensagem);
        threadServer.detach();
        sleep(1);
    }
}

Trem *MainWindow::createTrem(int id, int velocidade, int step, bool clockwise, QRect qtrem, QRect qtrilho){
    int x = qtrem.x();
    int y = qtrem.y();
    int top = qtrilho.y();
    int left = qtrilho.x();
    int right = left + qtrilho.width();
    int bottom = top + qtrilho.height();
    Trem *trem = new Trem(id, x, y, velocidade, step, clockwise, top, left,right,bottom);

    return trem;
}

void MainWindow::enableTrains(bool value){
    trem1->setEnable(value);
    trem2->setEnable(value);
    trem3->setEnable(value);
    trem4->setEnable(value);
    trem5->setEnable(value);
    trem6->setEnable(value);
}

void MainWindow::updateTrains(){
    switch (mensagem.trainID) {
        case -1:
            if(mensagem.travado){
                enableTrains(false);
            }else{
                enableTrains(true);
            }
        break;
        case 1:
            if(mensagem.speed != -1) trem1->setVelocidade(mensagem.speed);
            else
            trem1->setEnable(!mensagem.travado);
        break;
        case 2:
            if(mensagem.speed != -1) trem2->setVelocidade(mensagem.speed);
            else
            trem2->setEnable(!mensagem.travado);
        break;
        case 3:
            if(mensagem.speed != -1) trem3->setVelocidade(mensagem.speed);
            else
            trem3->setEnable(!mensagem.travado);
        break;
        case 4:
            if(mensagem.speed != -1) trem4->setVelocidade(mensagem.speed);
            else
            trem4->setEnable(!mensagem.travado);
        break;
        case 5:
            if(mensagem.speed != -1) trem5->setVelocidade(mensagem.speed);
            else
            trem5->setEnable(!mensagem.travado);
        break;
        case 6:
            if(mensagem.speed != -1) trem6->setVelocidade(mensagem.speed);
            else
            trem6->setEnable(!mensagem.travado);
        break;
        default:

        break;
    }
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow) {

    ui->setupUi(this);

    /*Criação dos Trens*/
    trem1 = createTrem(1, 40, 10, true, ui->labelTrem01->geometry(), ui->labelTrilho01->geometry());
    trem2 = createTrem(2, 50, 10, false, ui->labelTrem02->geometry(), ui->labelTrilho02->geometry());
    trem3 = createTrem(3, 60, 10, true, ui->labelTrem03->geometry(), ui->labelTrilho03->geometry());
    trem4 = createTrem(4, 30, 10, false, ui->labelTrem04->geometry(), ui->labelTrilho04->geometry());
    trem5 = createTrem(5, 50, 10, true, ui->labelTrem05->geometry(), ui->labelTrilho05->geometry());
    trem6 = createTrem(6, 60, 10, true, ui->labelTrem06->geometry(), ui->labelTrilho06->geometry());

    /*Criação dos Semaforos*/
    sem1 = new Semaforo(991,1,IPC_CREAT|0600);
    sem2 = new Semaforo(992,1,IPC_CREAT|0600);
    sem3 = new Semaforo(993,1,IPC_CREAT|0600);
    sem4 = new Semaforo(994,1,IPC_CREAT|0600);
    sem5 = new Semaforo(995,1,IPC_CREAT|0600);
    sem6 = new Semaforo(996,1,IPC_CREAT|0600);
    sem7 = new Semaforo(997,1,IPC_CREAT|0600);
    sem8 = new Semaforo(998,1,IPC_CREAT|0600);

    /*Criação das regiões críticas*/
    RegiaoCritica *regiao12 =  new RegiaoCritica(150, 20, 130, 120, sem1);
    RegiaoCritica *regiao14 =  new RegiaoCritica(160, 110, 70, 120, sem3);
    RegiaoCritica *regiao21 =  new RegiaoCritica(170, 20, 190, 120, sem1);
    RegiaoCritica *regiao24 =  new RegiaoCritica(160, 110, 300, 90, sem4);
    RegiaoCritica *regiao23 =  new RegiaoCritica(290, 120, 270, 20, sem2);
    RegiaoCritica *regiao34 =  new RegiaoCritica(380, 120, 300, 90, sem5);
    RegiaoCritica *regiao32 =  new RegiaoCritica(310, 120, 330, 20, sem2);
    RegiaoCritica *regiao45 =  new RegiaoCritica(90, 210, 260, 220, sem6);
    RegiaoCritica *regiao46 =  new RegiaoCritica(220, 220, 370, 190, sem7);
    RegiaoCritica *regiao43 =  new RegiaoCritica(370, 130, 270, 120, sem5);
    RegiaoCritica *regiao42 =  new RegiaoCritica(310, 120, 130, 120, sem4);
    RegiaoCritica *regiao41 =  new RegiaoCritica(170, 120, 90, 150, sem3);
    RegiaoCritica *regiao54 =  new RegiaoCritica(80, 220, 230, 250, sem6);
    RegiaoCritica *regiao56 =  new RegiaoCritica(220, 220, 200, 320, sem8);
    RegiaoCritica *regiao64 =  new RegiaoCritica(230, 230, 400, 220, sem7);
    RegiaoCritica *regiao65 =  new RegiaoCritica(240, 320, 260, 220, sem8);

    /*Inicialização das regiões críticas para cada trem*/
    trem1->addRegiaoCritica(regiao12,regiao14);
    trem2->addRegiaoCritica(regiao21,regiao24,regiao23);
    trem3->addRegiaoCritica(regiao34,regiao32);
    trem4->addRegiaoCritica(regiao45,regiao46,regiao43);
    trem4->addRegiaoCritica(regiao42,regiao41);
    trem5->addRegiaoCritica(regiao54,regiao56);
    trem6->addRegiaoCritica(regiao64,regiao65);

    /*Trens inicialmente parados*/
    enableTrains(false);

    /*Sinais para atualização da posição dos trens*/
    connect(trem1, SIGNAL(updateGUI(int, int, int)), SLOT(updateInterface(int, int, int)));
    trem1->start();
    connect(trem2, SIGNAL(updateGUI(int, int, int)), SLOT(updateInterface(int ,int, int)));
    trem2->start();
    connect(trem3, SIGNAL(updateGUI(int, int, int)), SLOT(updateInterface(int, int, int)));
    trem3->start();
    connect(trem4, SIGNAL(updateGUI(int,int, int)), SLOT(updateInterface(int, int, int)));
    trem4->start();
    connect(trem5, SIGNAL(updateGUI(int, int, int)), SLOT(updateInterface(int, int, int)));
    trem5->start();
    connect(trem6, SIGNAL(updateGUI(int, int, int)), SLOT(updateInterface(int, int, int)));
    trem6->start();

    /*Inicializaçã do servidor para escutar o cliente*/
    threadStartServer = std::thread(&MainWindow::server,this);

}

MainWindow::~MainWindow() {
    delete ui;
    ::close(socketId);
    threadServer.join();
    threadStartServer.join();
}

void MainWindow::updateInterface(int id, int x, int y) {
    switch(id){
        case 1:
            ui->labelTrem01->setGeometry(x, y, 20, 20);
            break;
        case 2:
            ui->labelTrem02->setGeometry(x, y, 20, 20);
            break;
        case 3:
            ui->labelTrem03->setGeometry(x, y, 20, 20);
            break;
        case 4:
            ui->labelTrem04->setGeometry(x, y, 20, 20);
            break;
        case 5:
            ui->labelTrem05->setGeometry(x, y, 20, 20);
            break;
        case 6:
            ui->labelTrem06->setGeometry(x, y, 20, 20);
            break;
        default:
            break;
    }

    if(sem1->getContador()) ui->lsem1->setStyleSheet("background-color:green;border-radius:8px;");
    else ui->lsem1->setStyleSheet("background-color:red;border-radius:8px;");
    if(sem2->getContador()) ui->lsem2->setStyleSheet("background-color:green;border-radius:8px;");
    else ui->lsem2->setStyleSheet("background-color:red;border-radius:8px;");
    if(sem3->getContador()) ui->lsem3->setStyleSheet("background-color:green;border-radius:8px;");
    else ui->lsem3->setStyleSheet("background-color:red;border-radius:8px;");
    if(sem4->getContador()) ui->lsem4->setStyleSheet("background-color:green;border-radius:8px;");
    else ui->lsem4->setStyleSheet("background-color:red;border-radius:8px;");
    if(sem5->getContador()) ui->lsem5->setStyleSheet("background-color:green;border-radius:8px;");
    else ui->lsem5->setStyleSheet("background-color:red;border-radius:8px;");
    if(sem6->getContador()) ui->lsem6->setStyleSheet("background-color:green;border-radius:8px;");
    else ui->lsem6->setStyleSheet("background-color:red;border-radius:8px;");
    if(sem7->getContador()) ui->lsem7->setStyleSheet("background-color:green;border-radius:8px;");
    else ui->lsem7->setStyleSheet("background-color:red;border-radius:8px;");
    if(sem8->getContador()) ui->lsem8->setStyleSheet("background-color:green;border-radius:8px;");
    else ui->lsem8->setStyleSheet("background-color:red;border-radius:8px;");



}
