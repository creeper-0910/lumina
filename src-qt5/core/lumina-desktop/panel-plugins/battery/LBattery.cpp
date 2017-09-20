//===========================================
//  Lumina-DE source code
//  Copyright (c) 2014, Susanne Jaeckel, 2015-2016 Ken Moore
//  Available under the 3-clause BSD license
//  See the LICENSE file for full details
//===========================================
#include "LBattery.h"
#include "LSession.h"

LBattery::LBattery(QWidget *parent, QString id, bool horizontal) : LPPlugin(parent, id, horizontal){
  iconOld = -1;
  //Setup the widget
  label = new QLabel(this);
    label->setScaledContents(true);
    //label->setAlignment(Qt::AlignCenter);
  this->layout()->addWidget(label);
  //Setup the timer
  timer = new QTimer();
  timer->setInterval(5000); //update every 5 seconds
  connect(timer,SIGNAL(timeout()), this, SLOT(updateBattery()) );
  timer->start();
  QTimer::singleShot(0,this,SLOT(OrientationChange()) ); //update the sizing/icon
}

LBattery::~LBattery(){
  timer->stop();
  delete timer;
}

void LBattery::updateBattery(bool force){
  // Get current state of charge
  //QStringList result = LUtils::getCmdOutput("/usr/sbin/apm", QStringList() << "-al");
  int charge = LOS::batteryCharge(); //result.at(1).toInt();
//qDebug() << "1: " << result.at(0).toInt() << " 2: " << result.at(1).toInt();
  int icon = -1;
  if (charge > 90) { icon = 4; }
  else if (charge > 70) { icon = 3; }
  else if (charge > 20) { icon = 2; }
  else if (charge > 5) { icon = 1; }
  else if (charge > 0 ) { icon = 0; }
  if(LOS::batteryIsCharging()){ icon = icon+10; }
  //icon = icon + result.at(0).toInt() * 10;
  if (icon != iconOld || force) {
    switch (icon) {
      case 0:
        label->setPixmap( LXDG::findIcon("battery-20","battery-caution").pixmap(label->size()) );
        break;
      case 1:
        label->setPixmap( LXDG::findIcon("battery-40", "battery-040").pixmap(label->size()) );
        break;
      case 2:
        label->setPixmap( LXDG::findIcon("battery-60", "battery-060").pixmap(label->size()) );
        break;
      case 3:
        label->setPixmap( LXDG::findIcon("battery-80", "battery-080").pixmap(label->size()) );
        break;
      case 4:
        label->setPixmap( LXDG::findIcon("battery-100", "battery").pixmap(label->size()) );
        break;
      case 10:
        label->setPixmap( LXDG::findIcon("battery-charging-20", "battery-charging-caution").pixmap(label->size()) );
        break;
      case 11:
        label->setPixmap( LXDG::findIcon("battery-charging-40", "battery-charging-040").pixmap(label->size()) );
        break;
      case 12:
        label->setPixmap( LXDG::findIcon("battery-charging-60", "battery-charging-060").pixmap(label->size()) );
        break;
      case 13:
        label->setPixmap( LXDG::findIcon("battery-charging-80", "battery-charging-080").pixmap(label->size()) );
        break;
      case 14:
        label->setPixmap( LXDG::findIcon("battery-charging-100", "battery-charging").pixmap(label->size()) );
        break;
      default:
        label->setPixmap( LXDG::findIcon("battery-unknown", "battery-missing").pixmap(label->size()) );
        break;
    }
    if(icon<iconOld && icon==0){
      //Play some audio warning chime when
      QString sfile = LSession::handle()->sessionSettings()->value("audiofiles/batterylow", LOS::LuminaShare()+"low-battery.ogg").toString();
      LSession::handle()->playAudioFile(sfile);
      }

    if(icon==0){ label->setStyleSheet("QLabel{ background: red;}"); }
    else if(icon==14 && charge>98){ label->setStyleSheet("QLabel{ background: green;}"); }
    else{ label->setStyleSheet("QLabel{ background: transparent;}"); }
    iconOld = icon;

  }
  //Now update the display
  QString tt;
  //Make sure the tooltip can be properly translated as necessary (Ken Moore 5/9/14)
  if(icon > 9 && icon < 15){ tt = QString(tr("%1 % (Charging)")).arg(QString::number(charge)); }
  else{ tt = QString( tr("%1 % (%2 Remaining)") ).arg(QString::number(charge), getRemainingTime() ); }
  label->setToolTip(tt);
}

QString LBattery::getRemainingTime(){
  int secs = LOS::batterySecondsLeft();
  if(secs < 0){ return "??"; }
  QString rem; //remaining
  if(secs > 3600){
    int hours = secs/3600;
    rem.append( QString::number(hours)+"h ");
    secs = secs - (hours*3600);
  }
  if(secs > 60){
    int min = secs/60;
    rem.append( QString::number(min)+"m ");
    secs = secs - (min*60);
  }
  if(secs > 0){
    rem.append(QString::number(secs)+"s");
  }
  return rem;
}
