<?php
require __DIR__ . '/Predis/Autoloader.php';
$mysql_host = 'localhost';
$mysql_db = 'mysql';
$PDO = null;
Predis\Autoloader::register();

//Aguarda conexao mysql
while (true) {
	try {
		$PDO = new PDO('mysql:host=' . $mysql_host . ';dbname=' . $mysql_db, 'root', '', array(
			PDO::ATTR_PERSISTENT => true,
		));
		break;
	} catch (PDOException $e) {
		echo 'Erro ao conectar com o MySQL: ' . $e->getMessage();
		file_put_contents('/opt/php_erro', $e->getMessage());
		sleep(3);
	}
}
//Aguarda conexao Redis
while (true) {
	try {
		$clientRedis = new Predis\Client();
		break;
	} catch (Exception $e) {
	echo 'Erro ao conectar com o Redis: ' . $e->getMessage();
		file_put_contents('/opt/php_erro', $e->getMessage());
		sleep(3);
	}
}

/* ClientID */
$clientMQTT = new Mosquitto\Client('MQTT_BD');

/* Credenciais (usuario e senha) */
$clientMQTT->setCredentials('ander', 'ander123');

/* Callback da conexao */
$clientMQTT->onConnect(function ($code, $message) use ($client) {
	/* Topico */
	$clientMQTT->subscribe('#', 0);
});

/* Callback da mensagem */
$client->onMessage(function ($message) use ($clientRedis) {
	/* exibe a mensagem e payload */
	echo $message->topic, "\n", $message->payload, "\n";
	//seta msg no redis
	if (!is_null($clientRedis)) {
		$clientRedis->($message->topic,$message->message);
	}
	//salva na tabela do mysql
	if (!is_null($PDO)) {
		$preparedStmt = $PDO->prepare('INSERT INTO MQTT (payload, topico) VALUES (?,?)');
		$preparedStmt->bindValue(1, $message->payload);
		$preparedStmt->bindValue(2, $message->topic);
		$preparedStmt->execute();
	}

});

/* Connecta ao host remoto */
$clientMQTT->connect('localhost', 1883);

/* Mantem-se em execucao */
$clientMQTT->loopForever();

