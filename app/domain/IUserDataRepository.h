#pragma once
#include "Entry.h"
#include "Note.h"
#include <QHash>
#include <QVector>

// Cong ra du lieu rieng cua nguoi dung: lich su, yeu thich, ghi chu.
// Tach khoi IDictionaryRepository vi vong doi va quyen ghi khac han:
// tu dien chi doc, cai nay doc-ghi.
class IUserDataRepository {
public:
    virtual ~IUserDataRepository() = default;

    // --- Lich su ---
    virtual void           addHistory(int entryId) = 0;
    virtual QVector<Entry> recentHistory(int limit) const = 0;

    // --- Yeu thich ---
    virtual bool           toggleFavorite(int entryId) = 0;   // tra ve trang thai MOI
    virtual bool           isFavorite(int entryId) const = 0;
    virtual QVector<Entry> favorites(int limit) const = 0;    // limit <= 0 => lay het

    // --- Ghi chu ---
    virtual QVector<Note>  notesFor(int entryId) const = 0;
    virtual int            addNote(const Note &note) = 0;      // tra ve id moi, 0 neu loi
    virtual bool           updateNote(const Note &note) = 0;
    virtual bool           deleteNote(int noteId) = 0;

    // Ghi chu cua tat ca tu yeu thich, gom san theo entryId.
    // Mot truy van duy nhat - tranh N+1 khi dung bo flashcard.
    virtual QHash<int, QVector<Note>> notesForFavorites(int limit) const = 0;
};
