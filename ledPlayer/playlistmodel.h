#ifndef PLAYLISTMODEL_H
#define PLAYLISTMODEL_H

#include <QAbstractItemModel>
#include <QMediaPlaylist>

/*
 * This class implements a playlist model to store songs to be played
 * it subclasses QAbstractItemModel to deal with song playlist using
 * a model/view approach. This is very common in visual applications
 * development since model implementation remains separated from
 * visual exibition aspects. For example, if the playlist is stored
 * in a database, it is possible execute commands to retrive the music
 * path from such database. Therefore, where the data is extracted is
 * responsability only from this class, and the viewing component does
 * not have to know nothing about this.
*/

class PlaylistModel : public QAbstractItemModel
{
  Q_OBJECT
private:
  // store the media playlist
  QMediaPlaylist *m_playlist;

  // maps the data index->variant
  // the index contains the playlist position the user
  // selected. The variant is a chunck of bytes used
  // to store some information. In this case, the song url
  // to be played
  QMap<QModelIndex, QVariant> m_data;
public:
  // class constructor
  explicit PlaylistModel(QObject *parent = 0);
  enum Column {
      Title = 0,
      ColumnCount
  };

signals:

public slots:
  // modelview assumes data is stored in a table structure
  // some of the methods below MUST BE IMPLEMENTED so the
  // data to be retrieved may be accessed correctly

  // returns the number of rows stored in model
  int rowCount(const QModelIndex &parent = QModelIndex()) const;

  // returns the number of columns stored in this model
  int columnCount(const QModelIndex &parent = QModelIndex()) const;

  // returns the model index of element at a given row/column
  QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const;

  // returns the parent of a given item
  QModelIndex parent(const QModelIndex &child) const;

  // returns the data associated with a given index.
  // The role variable tells the method what information is to be retrieved according
  // to a given role
  // Qt::DisplayRole is usually associated to some text that is stored
  QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

  // returns the playlist for the media player
  QMediaPlaylist *playlist() const;

  // tells what playlist shall be filled...
  void setPlaylist(QMediaPlaylist *playlist);

  // A setData is used to insert itens into the playlist
  bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::DisplayRole);

private slots:
  // some convenience functions to organize the internal structure of the list
  // they are used when a new playlist is to be defined
  // old playlist must be deleted an the new one has to be started up
  void beginInsertItems(int start, int end);
  void endInsertItems();
  void beginRemoveItems(int start, int end);
  void endRemoveItems();
  void changeItems(int start, int end);

};

#endif // PLAYLISTMODEL_H
