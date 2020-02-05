
# coding: utf-8

# In[1]:


import re
import sqlite3
import os
import sys
from functools import reduce


# In[2]:


def partition(l, p):
     return reduce(lambda x, y: x[not p(y)].append(y) or x, l, ([], []))

def get_numbers_in_line(l):
    fmt = r'\d+'
    return list(map(int, re.findall(fmt, l)))


# In[3]:


class ReduceLogParser:

    DEFAULT_REDUCE_LOG_FILE = 'reduce-stdout.txt'

    @staticmethod
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
                if len(curr_segment) > 0:
                    curr_segment.append(l)
                    segments.append(curr_segment)
                    curr_segment = []
                elif len(segments) > 0:
                    segments[-1].append(l)
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

    @staticmethod
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
        
        l4, l4_r = partition(l4, lambda l: re.match(r'^\*\*', l) is None)
        l5 = l4_r + [l5]
        l5 = "\n".join(l5)
        
        if (re.match(r'^This has ring-size', l1) is None) or (re.match(r'^\*\*', l5) is None):
            return None, None, None, None, [], [], "CRASHED", augment_depth
        
        [ring_size, n_col] = get_numbers_in_line(l1)
        [sima] = get_numbers_in_line(l2)
        [ncol_x] = get_numbers_in_line(l3)

        iter_log = list(map(get_numbers_in_line, l4))
        ncol_i_log = [x[0] for x in iter_log]
        sima_i_log = [x[1] for x in iter_log[:-1]]
        outcome = l5

        return ring_size, n_col, sima, ncol_x, ncol_i_log, sima_i_log, outcome, augment_depth

    @staticmethod
    def parse_reduce_log(exp_dir, log_file=DEFAULT_REDUCE_LOG_FILE):
        fname = os.path.join(exp_dir, log_file)
        if not os.path.isfile(fname):
            return None
        lines = []
        with open(fname, 'r') as f:
            lines = list(map(str.strip, f.readlines()))
        segments = ReduceLogParser.get_segments(lines)
        parsed_segments = list(map(ReduceLogParser.parse_config_verification, segments))
        crashed, parsed_segments = partition(parsed_segments, lambda l: l[6] == 'CRASHED')
        return parsed_segments, len(crashed) > 0


# In[4]:


class ConfigParser:
    DEFAULT_CONFIG_FILE = 'config.conf'

    @staticmethod
    def get_config_file_segments(lines):
        segments = []
        curr_segment = []
        for l in lines:
            if l == '':
                segments.append(curr_segment)
                curr_segment = []
            else:
                curr_segment.append(l)
        return segments

    @staticmethod
    def parse_config(lines):
        ID = lines[0]
        [N, R, A, B] = list(map(int, lines[1].split()))
        RC, *edges_in_x = list(map(int, lines[2].split()))
        if len(edges_in_x) != 2 * RC:
            print("Edges in X list not as expected. Expected {} numbers, got {}.".format(2 * RC, len(edges_in_x)))

        adjacency_list = []
        for v_id, v_desc in enumerate(lines[3:N + 3]):
            vertex_id, vertex_num_neighbors, *vertex_adjancency_list = list(map(int, v_desc.split()))
            if vertex_id != (v_id + 1):
                print("Vertex ID not as expected. Expected {}, got {}.".format(v_id + 1, vertex_id))
            if vertex_num_neighbors != len(vertex_adjancency_list):
                print("Adjacency list not as expected. Expected {} elements, got {}."
                      .format(len(vertex_adjancency_list), vertex_num_neighbors))
            adjacency_list.append(vertex_adjancency_list)

        positions = []
        for l in lines[N + 3:]:
            coords = list(map(int, l.split()))
            positions.extend(coords)

        if len(positions) != N:
            print("Number of positions parsed not as expected. Expected {}, got {}.".format(N, len(positions)))

        return ID, N, R, A, B, RC, adjacency_list, positions, edges_in_x

    @staticmethod
    def serialize_adjacency_list(adj_list):
        return ";".join(list(map(lambda x: ",".join(map(str, x)), adj_list)))

    @staticmethod
    def deserialize_adjacency_list(serialized_adj_list):
        return list(map(lambda x: list(map(int, x.split(','))), serialized_adj_list.split(";")))

    @staticmethod
    def serialize_vertex_positions(vertex_positions):
        return ",".join(map(str, vertex_positions))

    @staticmethod
    def deserialize_vertex_positions(serialized_vertex_positions):
        return list(map(int, serialized_vertex_positions.split(',')))

    @staticmethod
    def serialize_edges_in_x(edges_in_x):
        pairs = ["{},{}".format(str(edges_in_x[2 * i]), str(edges_in_x[2 * i + 1])) for i in
                 range(len(edges_in_x) // 2)]
        return ";".join(pairs)

    @staticmethod
    def deserialize_edges_in_x(serialized_edges_in_x):
        if serialized_edges_in_x == '':
            return []
        pairs = serialized_edges_in_x.split(';')
        edges_in_x = []
        for pair in pairs:
            pair = pair.split(',')
            pair = list(map(int, pair))
            edges_in_x.extend(pair)
        return edges_in_x

    @staticmethod
    def parse_configs_from_file(fname):
        with open(fname, 'r')  as f:
            lines = list(map(str.strip, f.readlines()))
        config_file_segments = ConfigParser.get_config_file_segments(lines)
        config_descs = list(map(ConfigParser.parse_config, config_file_segments))
        config_descs = list(map(lambda x:
                                (x[0], x[1], x[2], x[3], x[4], x[5],
                                 ConfigParser.serialize_adjacency_list(x[6]),
                                 ConfigParser.serialize_vertex_positions(x[7]),
                                 ConfigParser.serialize_edges_in_x(x[8])),
                                config_descs))
        return config_descs

    @staticmethod
    def parse_configs_from_exp_dir(exp_dir, config_file=DEFAULT_CONFIG_FILE):
        fname = os.path.join(exp_dir, config_file)
        if not os.path.isfile(fname):
            return []
        return ConfigParser.parse_configs_from_file(fname)


# In[5]:


class MetaDataParser:

    DEFAULT_COMPILER_INFO_FILE = 'cc.txt'
    DEFAULT_COMPILE_TIME_SYSTEM_INFO_FILE = 'compile_os.txt'
    DEFAULT_RUN_TIME_SYSTEM_INFO_FILE = 'runtime_os.txt'
    DEFAULT_SYSTEM_INFO_FILE = 'os.txt'
    DEFAULT_VERSION_NOTES_FILE = 'version_notes.txt'

    @staticmethod
    def parse_file(fname):
        if os.path.isfile(fname):
            with open(fname, 'r') as f:
                lines = map(str.strip, f.readlines())
                lines = list(filter(lambda x: len(x) > 0, lines))
            return "\n".join(lines)
        else:
            return None

    @staticmethod
    def parse_compile_time_system_details(exp_dir):
        t = MetaDataParser.parse_file(os.path.join(exp_dir, MetaDataParser.DEFAULT_COMPILE_TIME_SYSTEM_INFO_FILE))
        if t is None:
            t = MetaDataParser.parse_file(os.path.join(exp_dir, MetaDataParser.DEFAULT_SYSTEM_INFO_FILE))
        return t

    @staticmethod
    def parse_runtime_system_details(exp_dir):
        t = MetaDataParser.parse_file(os.path.join(exp_dir, MetaDataParser.DEFAULT_RUN_TIME_SYSTEM_INFO_FILE))
        if t is None:
            t = MetaDataParser.parse_file(os.path.join(exp_dir, MetaDataParser.DEFAULT_SYSTEM_INFO_FILE))
        return t

    @staticmethod
    def parse_compiler_details(exp_dir):
        return MetaDataParser.parse_file(os.path.join(exp_dir, MetaDataParser.DEFAULT_COMPILER_INFO_FILE))

    @staticmethod
    def parse_version_notes(exp_dir):
        return MetaDataParser.parse_file(os.path.join(exp_dir, MetaDataParser.DEFAULT_VERSION_NOTES_FILE))
            
    
    @staticmethod
    def extract_run_info(exp_dir, exp_name=None):

        return {
            'compile_time_sys': MetaDataParser.parse_compile_time_system_details(exp_dir),
            'run_time_sys': MetaDataParser.parse_runtime_system_details(exp_dir),
            'compiler': MetaDataParser.parse_compiler_details(exp_dir),
            'notes': MetaDataParser.parse_version_notes(exp_dir),
        }


# In[6]:


class SqliteHandler:

    create_reduceLog_table_sql = """ CREATE TABLE IF NOT EXISTS reduceLog (
                                                runID integer NOT NULL,
                                                configIndex integer NOT NULL,
                                                ringSize integer NOT NULL,
                                                ncol integer NOT NULL,
                                                sima integer NOT NULL,
                                                ncolx integer NOT NULL,
                                                outcome text,
                                                maxAugmentDepth integer,
                                                PRIMARY KEY (runID, configIndex)
                                            );
                                 """

    create_run_table_sql = """ CREATE TABLE IF NOT EXISTS run (
                                            runID integer NOT NULL,
                                            run_name text,
                                            compile_time_sys text,
                                            run_time_sys text,
                                            compiler text,
                                            notes text,
                                            PRIMARY KEY (runID)
                                        ); 
                                """

    create_progress_table_sql = """ CREATE TABLE IF NOT EXISTS progress (
                                            runID integer NOT NULL,
                                            configIndex integer NOT NULL,
                                            round integer NOT NULL,
                                            ncol_i integer,
                                            sima_i integer,
                                            PRIMARY KEY (runID, configIndex, round)
                                        ); 
                                 """

    create_config_table_sql = """ CREATE TABLE IF NOT EXISTS config (
                                            configIndex INTEGER PRIMARY KEY AUTOINCREMENT,
                                            configID text NOT NULL,
                                            numVertices integer NOT NULL,
                                            ringSize integer NOT NULL,
                                            ncolx integer NOT NULL,
                                            ncolxp integer NOT NULL,
                                            numEdgesX integer NOT NULL,
                                            adjacencyList text NOT NULL,
                                            vertexCoords text NOT NULL,
                                            edgesInX text NOT NULL,
                                            configSet text,
                                            UNIQUE (configID, numVertices, ringSize, ncolx, ncolxp, numEdgesX, adjacencyList, vertexCoords, edgesInX)
                                        ); 
                                 """

    @staticmethod
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

    @staticmethod
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

    @staticmethod
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

    @staticmethod
    def get_next_run_id(conn):
        return max([SqliteHandler.get_max_run_id_from_table(table_name, conn) for table_name in ['reduceLog', 'run', 'progress']]) + 1

    @staticmethod
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

    @staticmethod
    def get_config_index(conn, config_desc):

        config_exists_check_sql = """ SELECT configIndex FROM config WHERE
                                      configID = ? AND
                                      numVertices = ? AND
                                      ringSize = ? AND
                                      ncolx = ? AND
                                      ncolxp = ? AND
                                      numEdgesX = ? AND
                                      adjacencyList = ? AND
                                      vertexCoords = ? AND
                                      edgesInX = ?
                                  """

        try:
            c = conn.cursor()
            c = c.execute(config_exists_check_sql, config_desc)
            v = c.fetchall()
            if len(v) > 0:
                return v[0][0]
            else:
                return None
        except sqlite3.Error as e:
            print(e)

    @staticmethod
    def config_desc_to_kv_s(config_desc):
        configID, numVertices, ringSize, ncolx, ncolxp, numEdgesX, adjacencyList, vertexCoords, edgesInX = config_desc
        kv_s = {
            'configID': configID,
            'numVertices': numVertices,
            'ringSize': ringSize,
            'ncolx': ncolx,
            'ncolxp': ncolxp,
            'numEdgesX': numEdgesX,
            'adjacencyList': adjacencyList,
            'vertexCoords': vertexCoords,
            'edgesInX': edgesInX,
        }
        return kv_s

    @staticmethod
    def insert_config_into_table(conn, config_desc, config_set=None):

        kv_s = SqliteHandler.config_desc_to_kv_s(config_desc)
        configIndex = None
        if config_set is not None:
            kv_s['configSet'] = config_set
        try:
            configIndex = SqliteHandler.insert_in_table(conn, 'config', kv_s)
            return configIndex
        except sqlite3.Error as e:
            if "UNIQUE constraint failed" in e.args[0]:
                configIndex = SqliteHandler.get_config_index(conn, config_desc)
                return configIndex
            else:
                print(e)

    @staticmethod
    def insert_and_get_config_indices(conn, config_descs, config_set=None):
        return list(map(lambda x: SqliteHandler.insert_config_into_table(conn, x, config_set), config_descs))

    @staticmethod
    def insert_into_reduceLog_table(conn, db, run_id):

        for i, (configIndex, ring_size, n_col, sima, ncol_x, _, _, outcome, augment_depth) in enumerate(db):
            kv_s = {'runID': run_id,
                    'configIndex': configIndex,
                    'ringSize': ring_size,
                    'ncol': n_col,
                    'sima': sima,
                    'ncolx': ncol_x,
                    'outcome': outcome,
                    'maxAugmentDepth': augment_depth,
                    }
            SqliteHandler.insert_in_table(conn, 'reduceLog', kv_s)

    @staticmethod
    def insert_into_progress_table(conn, db, run_id):

        for i, (configIndex, _, _, _, _, ncol_iter_log, sima_iter_log, _, _) in enumerate(db):

            n = len(ncol_iter_log)
            for j in range(n - 1):
                kv_s = {
                    'runID': run_id,
                    'configIndex': configIndex,
                    'round': j + 1,
                    'ncol_i': ncol_iter_log[j],
                    'sima_i': sima_iter_log[j],
                }

                SqliteHandler.insert_in_table(conn, 'progress', kv_s)

            if n > 1:
                kv_s = {
                    'runID': run_id,
                    'configIndex': configIndex,
                    'round': n,
                    'ncol_i': ncol_iter_log[n - 1],
                }

            SqliteHandler.insert_in_table(conn, 'progress', kv_s)

    @staticmethod
    def insert_into_run_table(conn, run_info, run_id, run_name, crashed=False):

        run_info = run_info.copy()
        run_info['runID'] = run_id
        run_info['run_name'] = run_name
        if crashed:
            if ('notes' not in run_info) or (run_info['notes'] is None):
                run_info['notes'] = ''
            run_info['notes'] += '\nCRASHED'

        SqliteHandler.insert_in_table(conn, 'run', run_info)

    @staticmethod
    def setup_sqlite_db(db_name):
        conn = SqliteHandler.create_connection(db_name)
        SqliteHandler.create_table(conn, SqliteHandler.create_config_table_sql)
        SqliteHandler.create_table(conn, SqliteHandler.create_reduceLog_table_sql)
        SqliteHandler.create_table(conn, SqliteHandler.create_run_table_sql)
        SqliteHandler.create_table(conn, SqliteHandler.create_progress_table_sql)
        conn.commit()
        return conn


# In[7]:


def add_config_indices_to_reduce_log(reduce_log, config_indices):
    return list(map(lambda x: (x[0],*x[1]), zip(config_indices, reduce_log)))


# In[8]:


def get_run_name(exp_dir, exp_name=None):
    if exp_name:
        return exp_name
    else:
        if exp_dir[-1] == '/':
            exp_dir = exp_dir[:-1]
        return os.path.basename(exp_dir)


# In[9]:


def initialize_config_table(db_name):
    file_configSet_map = {
        os.path.join('config-files', 'JPS-2822.conf'): 'JPS',
        os.path.join('config-files', 'RSST-unavoidable.conf'): 'RSST',
    }
    conn = SqliteHandler.setup_sqlite_db(db_name)
    for config_f, config_set in file_configSet_map.items():
        config_descs = ConfigParser.parse_configs_from_file(config_f)
        config_indices = SqliteHandler.insert_and_get_config_indices(conn, config_descs, config_set)
    conn.commit()
    conn.close()


# In[10]:


def process_exp_dir(exp_dir, db_name, exp_name=None, config_set=None):
    conn = SqliteHandler.setup_sqlite_db(db_name)

    config_descs = ConfigParser.parse_configs_from_exp_dir(exp_dir)
    config_indices = SqliteHandler.insert_and_get_config_indices(conn, config_descs, config_set)
    conn.commit()
    log_db, crashed = ReduceLogParser.parse_reduce_log(exp_dir)
    log_db = add_config_indices_to_reduce_log(log_db, config_indices)
    run_info = MetaDataParser.extract_run_info(exp_dir, exp_name)

    try:
        run_id = SqliteHandler.get_next_run_id(conn)
        run_name = get_run_name(exp_dir, exp_name)
        SqliteHandler.insert_into_reduceLog_table(conn, log_db, run_id)
        SqliteHandler.insert_into_progress_table(conn, log_db, run_id)
        SqliteHandler.insert_into_run_table(conn, run_info, run_id, run_name, crashed)
#     except Exception as e:
#         print(e)
    finally:
        conn.commit()
        conn.close()




if __name__ == "__main__":

    if len(sys.argv) < 3:
        print("""Not enough arguments provided. Provide experiment directory (required), database name (required), experiment name to use in database (optional, folder name used if not provided) and name of configuration set (optional), in that order.""")
        exit(0)

    exp_dir = sys.argv[1]
    db_name = sys.argv[2]
    exp_name = sys.argv[3] if len(sys.argv) >= 4 else None
    config_set = sys.argv[4] if len(sys.argv) >= 5 else None

    try:
        initialize_config_table(db_name)
    except:
        print("Couldn't initialize database with known configuration sets.")

    process_exp_dir(exp_dir, db_name, exp_name, config_set)

