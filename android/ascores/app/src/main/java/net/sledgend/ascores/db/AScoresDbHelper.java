package net.sledgend.ascores.db;

import android.content.Context;
import android.database.sqlite.SQLiteDatabase;
import android.database.sqlite.SQLiteOpenHelper;

import net.sledgend.ascores.db.AScoresContract.RoundDefs;
import net.sledgend.ascores.db.AScoresContract.SavedScores;

/**
 * Created by Michael on 18/06/2016.
 */
public class AScoresDbHelper extends SQLiteOpenHelper {
    public static final int DATABASE_VERSION = 1;
    public static final String DATABASE_NAME = "AScores.db";

    private static final String COMMA_SEP = ", ";
    private static final String ROUND_TABLE_CREATE =
            "CREATE TABLE " + RoundDefs.TABLE_NAME + " (" +
                    RoundDefs._ID + " INTEGER" + "PRIMARY KEY AUTOINCREMENT" + COMMA_SEP +
                    RoundDefs.COLUMN_NAME_NAME + " TEXT" + COMMA_SEP +
                    RoundDefs.COLUMN_NAME_ARROWS_PER_END + " INTEGER" + COMMA_SEP +
                    RoundDefs.COLUMN_NAME_ENDS + " INTEGER" + COMMA_SEP +
                    RoundDefs.COLUMN_NAME_IMPERIAL + " INTEGER" + // Boolean
                    ");";
    private static final String SCORES_TABLE_CREATE =
            "CREATE TABLE " + SavedScores.TABLE_NAME + " (" +
                    SavedScores._ID + " INTEGER" + "PRIMARY KEY AUTOINCREMENT" + COMMA_SEP +
                    SavedScores.COLUMN_NAME_DATE + " TEXT" + COMMA_SEP +
                    SavedScores.COLUMN_NAME_ROUND + " INTEGER" + "NOT NULL" + COMMA_SEP + // RoundDefs._ID
                    SavedScores.COLUMN_NAME_SCORES + " BLOB" + COMMA_SEP +
                    "FOREIGN KEY(" + SavedScores.COLUMN_NAME_ROUND + ")" + " REFERENCES "
                    + RoundDefs.TABLE_NAME + "(" + RoundDefs._ID + ")" +
                    ");";

    public AScoresDbHelper(Context context) {
        super(context, DATABASE_NAME, null, DATABASE_VERSION);
    }

    public void onCreate(SQLiteDatabase db) {
        db.setForeignKeyConstraintsEnabled(true);
        db.execSQL(ROUND_TABLE_CREATE);
        db.execSQL(SCORES_TABLE_CREATE);
    }

    public void onUpgrade(SQLiteDatabase db, int oldVersion, int newVersion) {
        // nothing for now
    }
}
