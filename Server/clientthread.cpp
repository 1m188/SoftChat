#include "clientthread.h"

ClientThread::ClientThread(QObject *parent, qintptr handle)
	: QThread(parent), connectToClient(nullptr), handle(handle), id(""), headLen(sizeof(qint32)), msgLen(0), buffer(QByteArray())
{

}

ClientThread::~ClientThread()
{

}

void ClientThread::run()
{
	connectToClient = new QTcpSocket(nullptr);
	connectToClient->setSocketDescriptor(handle);
	connect(this, &QThread::finished, connectToClient, &QTcpSocket::deleteLater);
	connect(connectToClient, &QTcpSocket::readyRead, this, &ClientThread::getMsgFromClient);
	connect(this, &ClientThread::sendMsgToClientSignal, connectToClient, [&](QString msg) {connectToClient->write(msg.toUtf8()); connectToClient->waitForBytesWritten(); });
	exec();
}

void ClientThread::getMsgFromClient()
{
	//粘包解决！
	buffer.append(connectToClient->readAll());
	int totalLen = buffer.size(); //每次收到的数据全部拿过来到缓冲区后获得总长度
	//当总长度>=0的时候循环
	while (totalLen)
	{
		//用QDataStream来操纵QByteArray数据
		QDataStream in(&buffer, QIODevice::ReadOnly); //只读
		in.setByteOrder(QDataStream::BigEndian); //设置大端

		if (msgLen == 0) //当信息长度为0的时候，说明此时还没有读取任何信息的长度，所以此时应该读头
		{
			if (totalLen >= headLen) //如果总长>头长，读，不然跳出循环等后面的包
			{
				in >> msgLen; //读一波
				if (totalLen >= headLen + msgLen) //如果总长还大于头长+包身长的话，读出包身，作为真正的信息，不然跳出循环等下次更多的信息来
				{
					QString msg;
					in >> msg;
					emit getMsgFromClientSignal(msg);
					msgLen = 0; //把包身长=0，便于下次继续读头
					buffer = buffer.right(buffer.size() - headLen - msgLen); //更新缓冲区
					totalLen = buffer.size(); //更新总长度
				}
				else
				{
					break;
				}
			}
			else
			{
				break;
			}
		}
		else //否则，必定已经是读了包身长但是没有把包身信息读出来的
		{
			if (totalLen >= headLen + msgLen) //判断是否可读包身
			{
				QString msg;
				in >> msg;
				emit getMsgFromClientSignal(msg);
				msgLen = 0;
				buffer = buffer.right(buffer.size() - headLen - msgLen);
				totalLen = buffer.size();
			}
			else //长度不够的话跳出循环等下一次信息来临
			{
				break;
			}
		}
	}
}