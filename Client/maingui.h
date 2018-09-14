#ifndef MAINGUI_H
#define MAINGUI_H

#include <QWidget>
#include "QListWidget"
#include "chatframe.h"
#include "extern.h"

//主界面
class MainGui : public QWidget
{
	Q_OBJECT

public:
	MainGui(QWidget *parent);
	~MainGui();

private:
	UserInfo myInfo; //自己的用户信息
	QListWidget *friendList; //好友列表
	QWidget *framePlace; //聊天界面大小的Widget（用来在布局的时候占地，以及之后给聊天界面提供容器）
	QVector<ChatFrame *> chatFramePool; //聊天界面列表

protected:
	void resizeEvent(QResizeEvent *event) override; //重写改变大小事件，使得窗口大小改变的时候让聊天界面随之改变

signals:
	void sendMsgSignal(QString msg, QString recverID); //发送聊天消息
	void getMsgSignal(QString msg, QString senderID); //获取聊天消息

	private slots:
	void friendListItemDoubleClicked(QListWidgetItem *item); //双击好友列表中的某一项，显示相关的聊天界面
	void addFriendActionTriggered(); //添加好友
	void delFriendActionTriggered(); //删除好友
	void videoActionTriggered(); //视频聊天

	public slots:
	void getMyInfoSlot(QString id, QString name); //获取自己的用户信息
	void updateFriendList(QStringList friendList); //更新好友列表
	void getMsgSlot(QString msg, QString senderID); //获取聊天消息
};

#endif // MAINGUI_H
