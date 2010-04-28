// For conditions of distribution and use, see copyright notice in license.txt

#ifndef incl_Interfaces_CommunicationsService_h
#define incl_Interfaces_CommunicationsService_h

#include "ServiceInterface.h"
#include <QObject>
#include <Vector3D.h>
#include <QList>
#include <QByteArray>

class QString;
class QDateTime;

namespace Communications
{
    //! Naali Communications Framework 
    //! ==============================
    //!
    //! - Communications module implements CommunicationsService interface.
    //! - Modules can implement and registe their communication providers to CommunicationsService
    //! - Modules can request communications features from CommunicationsService
    //!
    //! Communication types
    //! - Private chat ..... Text message based communication between 2..N people
    //! - Public chat ...... Text message based communication on public channel
    //! - Voice call ....... Voice call between 2..N people
    //! - Video call ....... Video and audio call between 2..N people
    //! - In-world voice ... Spatial Voice communication in virtual reality 
    //! - In-world chat .... Spatial text message based communication
    //!  
    //! IM Services
    //! Connections to IM services can be opened and closed with credentials. Open connections
    //! can offer contact list with presence support. User can open only one connection per
    //! IM type such as irc and jabber.
    //! 
    //! - Contact list ..... List of contacts from certain IM service
    //!
    //!
    //!
    //!
    //!
    //!
    //!

    class VoiceCallInterface;       // \todo: Define       telepathy
    class VideoCallInterface;       // \todo: Define       telepathy
    class PublicChatSessionInterface;  // \todo: Define       telepathy
    class PrivateChatSessionInterface; // \todo: Define       telepathy

    class ContactInterface;         // \todo: Define       telepathy, opensim
    class ContactListInterface;     // \todo: Define       telepathy, opensim
    class InWorldVoiceInterface;    //                     mumble
    class InWorldTextChat;          //                     opensim

    //!
    class TextMessageInterface
    {
    public:
        virtual const QString& Author() const = 0;
        virtual const QString& Text() const = 0;
        virtual const QDateTime& TimeStamp() const = 0;
        virtual QList<QByteArray> Attachments() const = 0;
    };

    //namespace IM
    //{
    //    class FriendRequestInterface
    //    {
    //    public:
    //        virtual QString From() const = 0;
    //        virtual QString Message() const = 0;
    //        virtual QDateTime TimeStamp() const = 0;
    //    };

    //    class ContactInterface
    //    {
    //    public:
    //        virtual QString FullName() const = 0;
    //        virtual QString ShortName() const = 0;
    //        virtual QString Alias() const = 0;
    //        virtual void SetAlias(QString alias) const = 0;
    //        virtual void Remove() = 0;
    //    };

    //    class ContactListInterface
    //    {
    //    public:
    //        virtual ~ContactListInterface() {};
    //        virtual ContactInterface& SelfContact() = 0;
    //        virtual void SetPresenceStatus(QString status) = 0;
    //        virtual void SetPresenceText(QString status) = 0;
    //        virtual QString PresenceStatus() const = 0;
    //        virtual QString PresenceText() const = 0;
    //        virtual void SendFriendRequest(QString address, QString message) = 0;
    //    signals:
    //        void FriendRequest(FriendRequestInterface& request);
    //    };

    //} // IM

    namespace InWorldChat
    {
        class ParticipantInterface
        {
        public:
            virtual const QString &Name() const = 0;

        };

        class TextMessageInterface : public Communications::TextMessageInterface
        {
        public:
            virtual const ParticipantInterface& AuthorParticipant() const = 0;
        };

        class SessionInterface : public QObject
        {
            Q_OBJECT
        public:
            virtual void SendTextMessage(const QString &text);

        signals:
            void TextMessageReceived(const TextMessageInterface &message);
        };

        //! Provider in-world chat sessions
        class ProviderInterface
        {
        public:
            virtual SessionInterface* Session() = 0;
            virtual QString& Description() = 0;
        };

    } // InWorldChat

    namespace InWorldVoice
    {
        class ParticipantInterface : public QObject
        {
            Q_OBJECT
        public:
            virtual ~ParticipantInterface() {};
         //   virtual QString AvatarUUID() const = 0; // do we get this always ?
            virtual bool IsSpeaking() const = 0;
            virtual void Mute(bool mute) = 0;
            virtual bool IsMuted() const = 0;
            virtual Vector3df Position() const = 0;

            //! \return true if participant has left 
//            virtual bool IsLeft() const = 0; // \todo better name for method
        signals:
            void StartSpkeaking(); 
            void StopSpeaking();
  //          void Left();
        };
//        typedef shared_ptr<ParticipantInterface> ParticipantPtr;

        //! In-world voice session for virtual reality environments
        class SessionInterface : QObject
        {
            Q_OBJECT
        public:
            virtual ~SessionInterface() {};
            virtual QString Description() = 0;
            virtual bool IsSendingAudio() = 0;
            virtual bool IsReceivingAudio() = 0;

            virtual void EnableAudioSending() = 0;
            virtual void DisableAudioSending() = 0;
            virtual bool IsAudioSendingEnabled() = 0;
            virtual void EnableAudioReceiving() = 0;
            virtual void DisableAudioReceiving() = 0;
            virtual bool IsAudioReceivingEnabled() = 0;

            //virtual void SetSelfPosition(const vector3df& pos) = 0;

            virtual QList<Communications::InWorldVoice::ParticipantInterface*> Participants() const = 0;
        signals:
            void ParticipantJoined(ParticipantInterface* participant);
            void ParticipantLeft(ParticipantInterface* participant);
            void StartSendingAudio();
            void StopSendingAudio();
            void StartReceivingAudio();
            void StopReceivingAudio();
        };

        //! Provider in-world voice sessions
        class ProviderInterface : public QObject
        {
            Q_OBJECT
        public:
            virtual ~ProviderInterface() {};
            virtual Communications::InWorldVoice::SessionInterface* Session() = 0;
            virtual QString& Description() = 0;
        signals:
            void SessionAvailable();
          //  void SessionInavailable();
        };

    } // InWorldVoice

    class ServiceInterface : public Foundation::ServiceInterface , public QObject
    {
        Q_OBJECT
    public:
        static ServiceInterface* Instance();
        virtual ~ServiceInterface() {};

        virtual InWorldVoice::SessionInterface* InWorldVoiceSession() const = 0;
        virtual InWorldChat::SessionInterface* InWorldChatSession() const = 0;

        //! Registrationd methods for communication providers
        virtual void Register(InWorldVoice::ProviderInterface& provider) = 0;
        virtual void Register(InWorldChat::ProviderInterface& provider) = 0;

    signals:
        void InWorldVoiceSessionRequest(InWorldVoice::SessionInterface* session);
        void InWorldTextChatRequst(InWorldChat::SessionInterface* session);
        //void PrivateChatRequest(PrivateChat::Session session);
    };
    typedef boost::shared_ptr<ServiceInterface> ServicePtr;

} // Communications

#endif incl_Interfaces_CommunicationsService_h