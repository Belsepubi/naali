#include "ConnectionProvider.h"
#include <QtCore>
#include <TelepathyQt4/Types>
#include <TelepathyQt4/Debug__>
#include <TelepathyQt4/PendingReady>

namespace TelepathyIM
{
	ConnectionProvider::ConnectionProvider(Foundation::Framework* framework): framework_(framework), dbus_daemon_(NULL), state_(STATE_INITIALIZING)
	{
		// We want to start dbus daemon only on Windows platform
#ifdef WIN32
		StartDBusDaemon();
#endif

		InitializeTelepathyConnectionManager("gabble");
	}

	ConnectionProvider::~ConnectionProvider()
	{
		DeleteConnections();

// We want to stopt dbus daemon only on Windows platform
#ifdef WIN32
		StopDBusDaemon();
#endif
	}

	void ConnectionProvider::DeleteConnections()
	{
		for (ConnectionVector::iterator i = connections_.begin(); i != connections_.end(); ++i)
		{
			//! @todo Check that there is enough time to actual close the connection to servers
			(*i)->Close();
			delete *i;
			*i = NULL;
		}
		connections_.clear();
	}
		
	QStringList ConnectionProvider::GetSupportedProtocols() const
	{
		return supported_protocols_;
	}

	Communication::ConnectionInterface* ConnectionProvider::OpenConnection(const Communication::CredentialsInterface& credentials)
	{
		if (state_ != STATE_READY)
		{
			LogError("Cannot open IM connection because Telepathy IM connection provider is not ready.");
			throw Core::Exception("Telepathy IM connection provider is not ready.");
		}

		Connection* connection = new Connection(tp_connection_manager_, credentials);
		connections_.push_back(connection);

		//! @todo FIX THESE
		connect(connection, SIGNAL( ConnectionReady(Communication::ConnectionInterface&) ), SLOT( OnConnectionReady(Communication::ConnectionInterface&) ));
		connect(connection, SIGNAL( ConnectionClosed(Communication::ConnectionInterface&) ), SLOT( OnConnectionClosed(Communication::ConnectionInterface&) ));
		connect(connection, SIGNAL( ConnectionError(Communication::ConnectionInterface&) ), SLOT( OnConnectionError(Communication::ConnectionInterface&) ));

		return connection;
	}

	Communication::ConnectionVector ConnectionProvider::GetConnections() const
	{
		Communication::ConnectionVector v;
		for (ConnectionVector::const_iterator i = connections_.begin(); i != connections_.end(); ++i)
		{
			v.push_back(*i);
		}
		return v;
	}

	void ConnectionProvider::StartDBusDaemon()
	{
		QString path = "dbus\\dbus-daemon.exe";
		QString arguments = "--config-file=data\\session.conf";

		dbus_daemon_ = new QProcess(this);
		QStringList env = QProcess::systemEnvironment();
		QString env_item = "DBUS_SESSION_BUS_ADDRESS=tcp:host=127.0.0.1,port=";
		env_item.append( QString(DBUS_SESSION_PORT_, 10));
		env << env_item;
		dbus_daemon_->setEnvironment(env);
		
		connect( dbus_daemon_, SIGNAL(readyReadStandardOutput()), SLOT(OnDBusDaemonStdout()) );
		connect( dbus_daemon_, SIGNAL(finished(int)), SLOT(OnDBusDaemonExited(int)) );

		QString command = path.append(" ").append(arguments);
		dbus_daemon_->start(command);
		bool ok = dbus_daemon_->waitForStarted(500);
		if (!ok)
		{
			LogError("Cannot start dbus daemon process.");
			throw Core::Exception("Cannot start up dbus daemon process.");
		}

		// wait some time so that dbus daemon have a time to start up properly
		QTime wait_time = QTime::currentTime().addSecs(1);
		while( QTime::currentTime() < wait_time )
			QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
	}

	void ConnectionProvider::StopDBusDaemon()
	{
		if (dbus_daemon_)
		{
			dbus_daemon_->kill(); // terminate seems not to be enough
			bool ok = dbus_daemon_->waitForFinished(1000);
			if (!ok)
			{
				LogError("Cannot terminate dbus daemon process.");
				return;
			}
			delete dbus_daemon_;
			dbus_daemon_ = NULL;
		}
	}

	//! @todo support more that one connection manager 
	//!       current implementation uses only gabble
	void ConnectionProvider::InitializeTelepathyConnectionManager(const QString &name)
	{
		Tp::registerTypes();
		Tp::enableDebug(true);
		Tp::enableWarnings(true);
//		qRegisterMetaType<Tp::FarsightChannel::Status>(); // for streamed media

		tp_connection_manager_ = Tp::ConnectionManager::create(name);
		connect(tp_connection_manager_->becomeReady(Tp::ConnectionManager::FeatureCore), SIGNAL( finished(Tp::PendingOperation *) ), SLOT( OnConnectionManagerReady(Tp::PendingOperation*) ));
	}

	void ConnectionProvider::OnConnectionManagerReady(Tp::PendingOperation *op)
	{
		if (op->isError())
		{
			std::string message = "Cannot initialize Telepathy ConnectionManager object: ";
			message.append( op->errorMessage().toStdString() );
			LogError( message );

			QString reason;
			reason.append(message.c_str());

			state_ = STATE_ERROR;
			return;
		}

		LogInfo("Telepathy connection provider is ready.");
		state_ = STATE_READY;
		supported_protocols_.append("jabber");
		emit( ProtocolListUpdated(supported_protocols_) );
	}

	void ConnectionProvider::OnDBusDaemonStdout()
	{
		//! ???
	}

	void ConnectionProvider::OnDBusDaemonExited( int )
	{
		// ???
	}

	void ConnectionProvider::OnConnectionReady(Communication::ConnectionInterface &connection)
	{

	}

	void ConnectionProvider::OnConnectionClosed(Communication::ConnectionInterface &connection)
	{

	}

	void ConnectionProvider::OnConnectionError(Communication::ConnectionInterface &connection)
	{

	}

} // end of namespace: TelepathyIM