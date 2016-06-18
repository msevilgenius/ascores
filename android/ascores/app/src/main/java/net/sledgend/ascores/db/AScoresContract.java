package net.sledgend.ascores.db;

import android.provider.BaseColumns;

/**
 * Created by Michael on 18/06/2016.
 * Contract class defining database Schema
 */
public final class AScoresContract {

    public AScoresContract(){}

    public static abstract class RoundDefs implements BaseColumns {
        public static final String TABLE_NAME = "rounds";
        public static final String COLUMN_NAME_NAME = "name";
        public static final String COLUMN_NAME_ENDS = "numofends";
        public static final String COLUMN_NAME_ARROWS_PER_END = "arrowsperend";
        public static final String COLUMN_NAME_IMPERIAL = "imperial";
    }

    public static abstract class SavedScores implements BaseColumns {
        public static final String TABLE_NAME = "savedscores";
        public static final String COLUMN_NAME_DATE = "date";
        public static final String COLUMN_NAME_ROUND = "round";
        public static final String COLUMN_NAME_SCORES = "scores";
    }
}
