/*
Copyright: SCLE SFE
Contributor: Julien Pagès <j.parkouss@gmail.com>

This software is a computer program whose purpose is to test graphical
applications written with the QT framework (http://qt.digia.com/).

This software is governed by the CeCILL v2.1 license under French law and
abiding by the rules of distribution of free software.  You can  use, 
modify and/ or redistribute the software under the terms of the CeCILL
license as circulated by CEA, CNRS and INRIA at the following URL
"http://www.cecill.info". 

As a counterpart to the access to the source code and  rights to copy,
modify and redistribute granted by the license, users are provided only
with a limited warranty  and the software's author,  the holder of the
economic rights,  and the successive licensors  have only  limited
liability. 

In this respect, the user's attention is drawn to the risks associated
with loading,  using,  modifying and/or developing or reproducing the
software by the user in light of its specific status of free software,
that may mean  that it is complicated to manipulate,  and  that  also
therefore means  that it is reserved for developers  and  experienced
professionals having in-depth computer knowledge. Users are therefore
encouraged to load and test the software's suitability as regards their
requirements in conditions enabling the security of their systems and/or 
data to be ensured and,  more generally, to use and operate it in the 
same conditions as regards security. 

The fact that you are presently reading this means that you have had
knowledge of the CeCILL v2.1 license and that you accept its terms.
*/

#ifndef PLAYER_H
#define PLAYER_H

#include "jsonclient.h"
#include <QWidget>
#include <QModelIndex>
class DelayedResponse;
class QAbstractItemView;
/**
  * @brief Player est une spécialisation de JsonClient pour la gestion des objets manipuler à distance.
  *
  * Gère le référencement des QWidget que l'on veut manipulable, et définit les actions
  * possible par un client de test.
  */
class Player : public JsonClient
{
    Q_OBJECT
public:
    explicit Player(QIODevice * device, QObject *parent = 0);
    
    qulonglong registerObject(QObject * object);
    QObject * registeredObject(const qulonglong & id);

signals:
    void emit_object_set_properties(QObject * object, const QVariantMap & props);
    void emit_model_item_action(const QString &, QAbstractItemView *, const QModelIndex &);
    
public slots:
    /*
      Ces slots sont automatiquement transformés en commande disponibles
      pour les clients.

      Ils peuvent être de deux formes différentes :

      - QtJson::JsonObject commande(const QtJson::JsonObject & command);
      - DelayedResponse * commande(const QtJson::JsonObject & command);

      La première forme est une réponse directe, synchrone à une demande
      (pas de possibilité d'attente sans bloquage de l'application). La
      deuxième forme permet de passer outre cette limitation.

      */
    QtJson::JsonObject list_actions(const QtJson::JsonObject & command);

    QtJson::JsonObject widget_by_path(const QtJson::JsonObject & command);
    QtJson::JsonObject active_widget(const QtJson::JsonObject & command);
    QtJson::JsonObject object_properties(const QtJson::JsonObject & command);
    QtJson::JsonObject object_set_properties(const QtJson::JsonObject & command);
    QtJson::JsonObject widgets_list(const QtJson::JsonObject & command);
    QtJson::JsonObject widget_click(const QtJson::JsonObject & command);
    QtJson::JsonObject widget_close(const QtJson::JsonObject & command);
    DelayedResponse * drag_n_drop(const QtJson::JsonObject & command);
    QtJson::JsonObject model_items(const QtJson::JsonObject & command);
    QtJson::JsonObject model_item_action(const QtJson::JsonObject & command);
    QtJson::JsonObject model_gitem_action(const QtJson::JsonObject & command);
    QtJson::JsonObject desktop_screenshot(const QtJson::JsonObject & command);
    QtJson::JsonObject widget_keyclick(const QtJson::JsonObject & command);
    DelayedResponse * shortcut(const QtJson::JsonObject & command);
    QtJson::JsonObject tabbar_list(const QtJson::JsonObject & command);
    QtJson::JsonObject graphicsitems(const QtJson::JsonObject & command);
    QtJson::JsonObject gitem_properties(const QtJson::JsonObject & command);
    QtJson::JsonObject call_slot(const QtJson::JsonObject & command);
    QtJson::JsonObject widget_activate_focus(const QtJson::JsonObject & command);
    QtJson::JsonObject headerview_list(const QtJson::JsonObject & command);
    QtJson::JsonObject headerview_click(const QtJson::JsonObject & command);
    QtJson::JsonObject headerview_path_from_view(const QtJson::JsonObject & command);

    QtJson::JsonObject quit(const QtJson::JsonObject & command);

private slots:
    void objectDeleted(QObject * object);
    void _object_set_properties(QObject * object, const QVariantMap & props);
    void _model_item_action(const QString &, QAbstractItemView *, const QModelIndex &);

private:
    QHash<qulonglong, QObject*> m_registeredObjects;
};

/**
  * @brief Permet de retrouver un objet précédemment référencé par le Player.
  */
class ObjectLocatorContext {
public:
    ObjectLocatorContext(Player * player,
                         const QtJson::JsonObject & command,
                         const QString & objKey);
    virtual ~ObjectLocatorContext() {}

    qulonglong id;
    QObject * obj;
    QtJson::JsonObject lastError;
    inline bool hasError() { return ! lastError.isEmpty(); }
};

/**
  * @brief Permet de retrouver un widget (de type T) précédemment référencé par le Player.
  */
template <class T = QWidget>
class WidgetLocatorContext : public ObjectLocatorContext {
public:
    WidgetLocatorContext(Player * player,
                         const QtJson::JsonObject & command,
                         const QString & objKey) : ObjectLocatorContext(player, command, objKey) {

        if (! hasError()) {
            widget = qobject_cast<T *>(obj);
            if (!widget) {
                lastError = player->createError("NotAWidget",
                                                QString::fromUtf8("L'objet (id:%1) n'est pas un %2").arg(id).arg(T::staticMetaObject.className()));
            }
        }
    }
    T * widget;
};

#endif // PLAYER_H
