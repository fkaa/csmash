pkgdatadir = $(datadir)/games/@PACKAGE@
localedir = $(prefix)/share/locale

bin_PROGRAMS = csmash csmashrobot

csmash_SOURCES = main.cpp LobbyClient.cpp Ball.cpp Event.cpp \
		Control.cpp View.cpp BaseView.cpp FieldView.cpp BallView.cpp \
		Player.cpp parts.cpp PlayerView.cpp HitMark.cpp \
		PenAttack.cpp ShakeCut.cpp PenDrive.cpp \
		ComTrainingPenAttack.cpp ComTrainingPenDrive.cpp \
		NetPenAttack.cpp NetPenDrive.cpp \
		NetShakeCut.cpp PlayerSelect.cpp PlayerSelectView.cpp \
		TrainingSelect.cpp TrainingSelectView.cpp PlayGame.cpp \
		SoloPlay.cpp PracticePlay.cpp PlayGameView.cpp Title.cpp \
		TitleView.cpp Howto.cpp HowtoView.cpp Training.cpp \
		TrainingView.cpp Opening.cpp OpeningView.cpp Sound.cpp \
		LoadImage.cpp z.cpp MultiPlay.cpp Network.cpp \
		Logging.cpp affine float matrix xerror.cpp \
		PracticeSelect.cpp PracticeSelectView.cpp BaseView2D.cpp \
		FieldView2D.cpp PlayerView2D.cpp BallView2D.cpp \
		PlayerSelectView2D.cpp TitleView2D.cpp \
		RCFile.cpp loadparts.cpp \
		MultiPlayerSelect.cpp glARB.cpp ttinc.cpp \
		LogPlay.cpp NoLogging.cpp Controller.cpp HumanController.cpp \
		ComController.cpp ComPenAttackController.cpp ComPenDriveController.cpp \
		ComShakeCutController.cpp ComTrainingPenAttackController.cpp \
		ComTrainingPenDriveController.cpp TrainingHumanController.cpp


noinst_HEADERS = ttinc.h LobbyClient.h Ball.h Event.h Control.h \
		View.h BaseView.h FieldView.h BallView.h Player.h \
		parts.h PlayerView.h HitMark.h PenAttack.h ShakeCut.h \
		PenDrive.h ComTrainingPenAttack.h ComTrainingPenDrive.h \
		NetPenAttack.h NetPenDrive.h NetShakeCut.h \
		PlayerSelect.h PlayerSelectView.h TrainingSelect.h \
		TrainingSelectView.h PlayGame.h SoloPlay.h PracticePlay.h \
		PlayGameView.h Title.h TitleView.h Howto.h HowtoView.h \
		Training.h TrainingView.h Opening.h OpeningView.h Sound.h \
		LoadImage.h z.h MultiPlay.h Network.h \
		Logging.h PracticeSelect.h \
		PracticeSelectView.h BaseView2D.h FieldView2D.h \
		PlayerView2D.h BallView2D.h PlayerSelectView2D.h \
		TitleView2D.h RCFile.h loadparts.h \
		MultiPlayerSelect.h glARB.h CEGUIKey.h \
		LogPlay.h NoLogging.h Controller.h HumanController.h \
		ComController.h ComPenAttackController.h ComPenDriveController.h \
		ComShakeCutController.h ComTrainingPenAttackController.h \
		ComTrainingPenDriveController.h TrainingHumanController.cpp

csmashrobot_SOURCES = \
              main.cpp LobbyClient.cpp Ball.cpp Event.cpp \
              Control.cpp Dummy.cpp \
              Player.cpp \
              PenAttack.cpp ShakeCut.cpp PenDrive.cpp \
              ComTrainingPenAttack.cpp ComTrainingPenDrive.cpp \
              NetPenAttack.cpp NetPenDrive.cpp \
              NetShakeCut.cpp PlayerSelect.cpp \
              TrainingSelect.cpp PlayGame.cpp \
              SoloPlay.cpp PracticePlay.cpp Title.cpp \
              Howto.cpp Training.cpp \
              Opening.cpp \
              z.cpp MultiPlay.cpp Network.cpp \
              Logging.cpp affine float matrix xerror.cpp \
              PracticeSelect.cpp \
              RCFile.cpp \
              glARB.cpp ttinc.cpp 

#csmashrobot_LDFLAGS = -static

csmash_LDADD = @LIBINTL@ @LIBS@
csmashrobot_LDADD = -lz -lpthread -lm -lSDL -ldl -lasound -lX11 -lXext -lvga -lncurses -lslang -lgpm -lgcrypt

DEFS = -DLOCALEDIR=\"$(localedir)\" -DCANNONSMASH_DATADIR=\"$(pkgdatadir)\" @DEFS@
LIBS = @LIBINTL@ @LIBS@
#INCLUDES = $(INCLUDES) -I../intl -I$(top_srcdir)/intl

SUBDIRS = m4  Parts images wav win32 po
EXTRA_DIST = config.rpath README.en README.nt Makefile.nt CREDITS csmash.spec

ACLOCAL_AMFLAGS = -I m4
