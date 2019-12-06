import re
import sqlite3
import os
import sys

create_config_table_sql = """ CREATE TABLE IF NOT EXISTS config (
                                        runID integer NOT NULL,
                                        configID integer NOT NULL,
                                        ringSize integer NOT NULL,
                                        ncol integer NOT NULL,
                                        sima integer NOT NULL,
                                        ncolx integer NOT NULL,
                                        outcome text,
                                        maxAugmentDepth integer,
                                        PRIMARY KEY (runID, configID)
                                    );
                           """

create_run_table_sql =     """ CREATE TABLE IF NOT EXISTS run (
                                        runID integer NOT NULL,
                                        compile_time_sys text,
                                        run_time_sys text,
                                        compiler text,
                                        notes text,
                                        PRIMARY KEY (runID)
                                    ); 
                            """

create_progress_table_sql = """ CREATE TABLE IF NOT EXISTS progress (
                                        runID integer NOT NULL,
                                        configID integer NOT NULL,
                                        round integer NOT NULL,
                                        ncol_i integer,
                                        sima_i integer,
                                        PRIMARY KEY (runID, configID, round)
                                    ); 
                             """

DEFAULT_COMPILER_INFO_FILE = 'cc.txt'
DEFAULT_COMPILE_TIME_SYSTEM_INFO_FILE = 'compile_os.txt'
DEFAULT_RUN_TIME_SYSTEM_INFO_FILE = 'runtime_os.txt'
DEFAULT_SYSTEM_INFO_FILE = 'os.txt'
DEFAULT_VERSION_NOTES_FILE = 'version_notes.txt'
DEFAULT_REDUCE_LOG_FILE = 'reduce-stdout.txt'


def get_numbers_in_line(l):
    fmt = r'\d+'
    return list(map(int, re.findall(fmt, l)))


def parse_config_verification(config_lines):
    config_lines = list(filter(lambda x: len(x.strip()) > 0, config_lines))

    augment_depth = None
    if re.match(r'^Configuration \d+: maximum', config_lines[-1]):
        [_, augment_depth] = get_numbers_in_line(config_lines[-1])
        config_lines = config_lines[:-1]
    elif re.match(r'^Configuration \d+: augment depth 8 >= 8 \(would crash if not patched\)$', config_lines[-1]):
        augment_depth = 8
        config_lines = config_lines[:-1]

    l1, l2, l3, _, _, *l4, l5 = config_lines

    [ring_size, n_col] = get_numbers_in_line(l1)
    [sima] = get_numbers_in_line(l2)
    [ncol_x] = get_numbers_in_line(l3)

    iter_log = list(map(get_numbers_in_line, l4))
    ncol_i_log = [x[0] for x in iter_log]
    sima_i_log = [x[1] for x in iter_log[:-1]]
    outcome = l5

    return ring_size, n_col, sima, ncol_x, ncol_i_log, sima_i_log, outcome, augment_depth


def get_segments(lines):
    segments = []
    curr_segment = []

    collect = False

    for i, l in enumerate(lines):

        if re.match(r'^\-+$', l) is not None:
            if len(curr_segment) > 0:
                segments.append(curr_segment)
                curr_segment = []
            collect = False
        elif re.match(r'^Configuration \d+$', l) is not None:
            if len(curr_segment) > 0:
                segments.append(curr_segment)
                curr_segment = []
            collect = False
        elif re.match(r'^This has ring-size', l) is not None:
            if len(curr_segment) > 0:
                segments.append(curr_segment)
                curr_segment = []
            curr_segment.append(l)
            collect = True
        elif re.match(r'^\*\*', l) is not None:
            curr_segment.append(l)
            segments.append(curr_segment)
            curr_segment = []
            collect = False
        elif re.match(r'^Configuration \d+: maximum', l) is not None:
            if len(curr_segment) > 0:
                curr_segment.append(l)
                segments.append(curr_segment)
                curr_segment = []
            elif len(segments) > 0:
                segments[-1].append(l)
            collect = False
            curr_segment = []
        elif re.match(r'^Configuration \d+: augment depth 8 >= 8 \(would crash if not patched\)$', l) is not None:
            if len(curr_segment) > 0:
                curr_segment.append(l)
                segments.append(curr_segment)
                curr_segment = []
            elif len(segments) > 0:
                segments[-1].append(l)
            collect = False
            curr_segment = []
        elif re.match(r'^Reducibility of', l) is not None:
            collect = False
            break
        elif collect and len(l.strip()) > 0:
            curr_segment.append(l)

    if len(curr_segment) > 0:
        segments.append(curr_segment)

    return segments


def create_connection(db_file):
    """ create a database connection to the SQLite database
        specified by db_file
    :param db_file: database file
    :return: Connection object or None
    """
    conn = None
    try:
        conn = sqlite3.connect(db_file)
        return conn
    except sqlite3.Error as e:
        print(e)

    return conn


def create_table(conn, create_table_sql):
    """ create a table from the create_table_sql statement
    :param conn: Connection object
    :param create_table_sql: a CREATE TABLE statement
    :return:
    """
    try:
        c = conn.cursor()
        c.execute(create_table_sql)
    except sqlite3.Error as e:
        print(e)


def get_max_run_id_from_table(table_name, conn):
    max_run_id_query = "SELECT MAX(runID) FROM {}".format(table_name)
    try:
        c = conn.cursor()
        c = c.execute(max_run_id_query)
        v = c.fetchall()[0][0]
        if v:
            return v
        else:
            return 0
    except sqlite3.Error as e:
        print(e)


def get_next_run_id(conn):
    return max([get_max_run_id_from_table(table_name, conn) for table_name in ['config', 'run', 'progress']]) + 1


def insert_in_table(conn, table_name, key_value_pairs):
    kv_s = key_value_pairs.items()
    keys = [x[0] for x in kv_s]
    values = [x[1] for x in kv_s]
    insertion_sql_query = """ INSERT INTO {0}({1})
                              VALUES({2})
                          """.format(table_name, ",".join(keys), ",".join(['?' for _ in values]))
    cur = conn.cursor()
    cur.execute(insertion_sql_query, values)
    return cur.lastrowid


def parse_file(fname):
    if os.path.isfile(fname):
        with open(fname, 'r') as f:
            lines = map(str.strip, f.readlines())
            lines = list(filter(lambda x: len(x) > 0, lines))
        return "\n".join(lines)
    else:
        return None


def parse_compile_time_system_details(exp_dir):
    t = parse_file(os.path.join(exp_dir, DEFAULT_COMPILE_TIME_SYSTEM_INFO_FILE))
    if t is None:
        t = parse_file(os.path.join(exp_dir, DEFAULT_SYSTEM_INFO_FILE))
    return t


def parse_runtime_system_details(exp_dir):
    t = parse_file(os.path.join(exp_dir, DEFAULT_RUN_TIME_SYSTEM_INFO_FILE))
    if t is None:
        t = parse_file(os.path.join(exp_dir, DEFAULT_SYSTEM_INFO_FILE))
    return t


def parse_compiler_details(exp_dir):
    return parse_file(os.path.join(exp_dir, DEFAULT_COMPILER_INFO_FILE))


def parse_version_notes(exp_dir):
    return parse_file(os.path.join(exp_dir, DEFAULT_VERSION_NOTES_FILE))


def extract_run_info(exp_dir):
    return {
        'compile_time_sys': parse_compile_time_system_details(exp_dir),
        'run_time_sys': parse_runtime_system_details(exp_dir),
        'compiler': parse_compiler_details(exp_dir),
        'notes': parse_version_notes(exp_dir),
    }

def parse_reduce_log(exp_dir, log_file=DEFAULT_REDUCE_LOG_FILE):
    fname = os.path.join(exp_dir, log_file)
    if not os.path.isfile(fname):
        return None
    lines = []
    with open(fname, 'r') as f:
        lines = list(map(str.strip, f.readlines()))
    segments = get_segments(lines)
    return list(map(parse_config_verification, segments))


def insert_into_config_table(conn, db, run_id):
    for i, (ring_size, n_col, sima, ncol_x, _, _, outcome, augment_depth) in enumerate(db):
        kv_s = {'runID': run_id,
                'configID': i + 1,
                'ringSize': ring_size,
                'ncol': n_col,
                'sima': sima,
                'ncolx': ncol_x,
                'outcome': outcome,
                'maxAugmentDepth': augment_depth,
                }
        insert_in_table(conn, 'config', kv_s)


def insert_into_progress_table(conn, db, run_id):
    for i, (_, _, _, _, ncol_iter_log, sima_iter_log, _, _) in enumerate(db):

        n = len(ncol_iter_log)
        for j in range(n - 1):
            kv_s = {
                'runID': run_id,
                'configID': i + 1,
                'round': j + 1,
                'ncol_i': ncol_iter_log[j],
                'sima_i': sima_iter_log[j],
            }

            insert_in_table(conn, 'progress', kv_s)

        kv_s = {
            'runID': run_id,
            'configID': i + 1,
            'round': n,
            'ncol_i': ncol_iter_log[n - 1],
        }

        insert_in_table(conn, 'progress', kv_s)


def insert_into_run_table(conn, run_info, run_id):
    run_info = run_info.copy()
    run_info['runID'] = run_id
    insert_in_table(conn, 'run', run_info)

def setup_sqlite_db(db_name):
    conn = create_connection(db_name, )
    create_table(conn, create_config_table_sql)
    create_table(conn, create_run_table_sql)
    create_table(conn, create_progress_table_sql)
    conn.commit()
    return conn

def process_exp_dir(exp_dir, db_name):
    log_db = parse_reduce_log(exp_dir)
    run_info = extract_run_info(exp_dir)
    conn = setup_sqlite_db(db_name)
    run_id = get_next_run_id(conn)
    insert_into_config_table(conn, log_db, run_id)
    insert_into_progress_table(conn, log_db, run_id)
    insert_into_run_table(conn, run_info, run_id)
    conn.commit()
    conn.close()

if len(sys.argv) < 3:
    print("Not enough arguments provided")
process_exp_dir(sys.argv[1], sys.argv[2])


