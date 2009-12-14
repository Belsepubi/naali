// For conditions of distribution and use, see copyright notice in license.txt

#include "StableHeaders.h"
#include "SessionHelper.h"
#include "FriendListWidget.h"
#include "ChatSessionWidget.h"
#include "VideoSessionWidget.h"
#include "UiDefines.h"

#include <QDebug>
#include <QPair>
#include <QPixmap>
#include <QInputDialog>

namespace UiHelpers
{
    SessionHelper::SessionHelper(QWidget *main_parent, Communication::ConnectionInterface  *im_connection)
        : QObject(),
          main_parent_(main_parent),
          im_connection_(im_connection),
          session_manager_ui_(0),
          friend_list_widget_(0),
          info_widget_(0),
          presence_status_(""),
          presence_status_message_(""),
          welcome_tab_destroyed(false)
    {

    }

    SessionHelper::~SessionHelper()
    {

    }

    /************* SET IT *************/

    void SessionHelper::SetStatusMessage()
    {
        bool ok = false;
        QString new_status_message = QInputDialog::getText(0, "New Status Message", "Give your new status message", QLineEdit::Normal, "", &ok);
        if (ok)
            SetMyStatusMessage(new_status_message);
    }

    void SessionHelper::SetMyStatus(const QString &status_code)
    {
        if (status_code != presence_status_)
        {
            im_connection_->SetPresenceStatus(status_code);
            presence_status_ = status_code;
            QPixmap status_pixmap(UiDefines::PresenceStatus::GetImagePathForStatusCode(status_code));
            session_manager_ui_->statusIconLabel->setPixmap(status_pixmap.scaled(24, 24, Qt::KeepAspectRatio));

            if (UiDefines::PresenceStatus::GetStatusCodeForStatus(friend_list_widget_->GetStatus()) != status_code)
                friend_list_widget_->SetStatus(status_code);

            emit ChangeMenuBarStatus(status_code);
        }
    }

    void SessionHelper::SetMyStatusMessage(const QString &status_message)
    {
        im_connection_->SetPresenceMessage(status_message);
        presence_status_message_ = status_message;
        session_manager_ui_->statusMessageLabel->setText(status_message);
    }

    /************* GET IT *************/

    QString SessionHelper::GetChatInviteSendersName(Communication::ChatSessionParticipantVector participant_vector)
    {
        Communication::ChatSessionParticipantVector::const_iterator iter;
        for( iter=participant_vector.begin(); iter!=participant_vector.end(); iter++ )
	    {
            Communication::ChatSessionParticipantInterface *participant = (*iter);
            if (participant->GetID() != my_name_)
                return participant->GetID();
        }
        return "";
    }

    QString SessionHelper::GetVideoInviteSendersName(Communication::VoiceSessionParticipantVector participant_vector)
    {
        Communication::VoiceSessionParticipantVector::const_iterator iter;
        for( iter=participant_vector.begin(); iter!=participant_vector.end(); iter++ )
	    {
            Communication::VoiceSessionParticipantInterface *participant = (*iter);
            if (participant->GetID() != my_name_)
                return participant->GetID();
        }
        return "";
    }

    /************* CHECK IT *************/

    bool SessionHelper::DoesChatTabExist(const QString &chat_friends_name)
	{
        if (chat_sessions_pointers_map_.contains(chat_friends_name))
        {
            QWidget *found_tab_widget = chat_sessions_pointers_map_[chat_friends_name].first;
            session_manager_ui_->sessionsTabWidget->setCurrentWidget(found_tab_widget);
            return true;
        }
		return false;
	}

    void SessionHelper::TabWidgetStateCheck()
    {
        if (!welcome_tab_destroyed)
        {
            session_manager_ui_->sessionsTabWidget->clear();
            welcome_tab_destroyed = true;
        }

        if (info_widget_)
            SAFE_DELETE(info_widget_);
    }

    /************* DO IT *************/

    void SessionHelper::CloseTab(const QString &chat_friends_name)
    {
        int tab_count = session_manager_ui_->sessionsTabWidget->count();
		for (int index = 0; index < tab_count; ++index)
			if ( QString::compare(session_manager_ui_->sessionsTabWidget->tabText(index), chat_friends_name) == 0 )
				session_manager_ui_->sessionsTabWidget->removeTab(index);

        // If that was the last tab, lets put some content so it wont look stupid
        if (session_manager_ui_->sessionsTabWidget->count() == 0)
        {
            SAFE_DELETE(info_widget_);
            info_widget_ = new QWidget();
            info_widget_->setObjectName("infoWidget");
            info_widget_->setStyleSheet(QString("QWidget#infoWidget { background-color: rgb(255,255,255); } QLabel { color: rgb(0,0,0); }"));
            QVBoxLayout *layout = new QVBoxLayout(info_widget_);
            layout->addSpacerItem(new QSpacerItem(1,1, QSizePolicy::Preferred, QSizePolicy::Expanding));
            layout->addWidget(new QLabel("Click on Show Friend List from the top menu to start communicating...", info_widget_));
            layout->addSpacerItem(new QSpacerItem(1,1, QSizePolicy::Preferred, QSizePolicy::Expanding));
            info_widget_->setLayout(layout);

            session_manager_ui_->sessionsTabWidget->addTab(info_widget_, "  Communications");
        }
    }

    void SessionHelper::CreateNewChatSessionWidget(Communication::ChatSessionInterface *chat_session, QString &chat_friends_name)
    {
        TabWidgetStateCheck();

        if (!DoesChatTabExist(chat_friends_name))
        {
            // Add tab and connections
            CommunicationUI::ChatSessionWidget *chat_session_tab = new CommunicationUI::ChatSessionWidget(main_parent_, chat_session, my_name_, chat_friends_name);
            int index = session_manager_ui_->sessionsTabWidget->addTab(chat_session_tab, QIcon(":images/iconChat.png"), chat_friends_name);
            session_manager_ui_->sessionsTabWidget->setCurrentIndex(index);
            connect(chat_session_tab, SIGNAL( Closed(const QString &) ), this, SLOT( CloseTab(const QString &) ));

            // Store to local container
            chat_sessions_pointers_map_[chat_friends_name].first = chat_session_tab;
            chat_sessions_pointers_map_[chat_friends_name].second = chat_session;
        }
    }

    void SessionHelper::CreateNewVideoSessionWidget(Communication::VoiceSessionInterface *video_session, QString &chat_friends_name)
    {
        TabWidgetStateCheck();

        CommunicationUI::VideoSessionWidget *video_session_tab = new CommunicationUI::VideoSessionWidget(main_parent_, video_session, my_name_, chat_friends_name);

        QPair<WId, WId> video_widget_ids = QPair<WId, WId>();
        video_widget_ids.first = video_session_tab->local_video_->winId();
        video_widget_ids.second = video_session_tab->remote_video_->winId();

        int index = session_manager_ui_->sessionsTabWidget->addTab(video_session_tab, QIcon(":images/iconChat.png"), chat_friends_name);
        session_manager_ui_->sessionsTabWidget->setCurrentIndex(index);
    }
}