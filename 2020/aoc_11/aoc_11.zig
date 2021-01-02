const print = @import("std").debug.print;
const fs = @import("std").fs;
const std = @import("std");


const LineReader = struct {
    file: fs.File,
    buffer: [255]u8,
    eofReached: bool,

    pub fn init(file: fs.File) LineReader {
        return LineReader {
            .file = file,
            .buffer = undefined,
            .eofReached = false,
        };
    }

    pub fn next_line(self: *LineReader) !?[]const u8
    {
        if (self.eofReached) {
            return null;
        }

        var index: usize = 0;
        var reader = self.file.reader();
        while (true) {
            const byte = reader.readByte() catch |err| switch (err) {
                error.EndOfStream => {
                    self.eofReached = true;
                    break;
                },
                else  => return err,
            };

            if (index >= self.buffer.len) {
                @panic("Line too long");
            }
            if (byte == '\n') {
                break;
            } else if (byte == '\r') {
                _ = try reader.readByte();
                break;
            } else {
                self.buffer[index] = byte;
            }
            index += 1;
        }

        if (index == 0)
        {
            return null;
        }
        return self.buffer[0..index];
    }
};


const CellType = enum {
    Floor,
    EmptySeat,
    OccupiedSeat,

    pub fn from_byte(byte: u8) ?CellType {
        const t = switch (byte) {
            'L' => CellType.EmptySeat,
            '.' => CellType.Floor,
            '#' => CellType.OccupiedSeat,
            else => null,
        };
        return t;
    }
};

const Map2D = struct {
    num_rows: usize,
    num_cols: usize,
    cells: std.ArrayList(CellType),


    pub fn init(num_rows: usize, num_cols: usize, allocator: *Allocator) Map2D {
        return Map2D {
            .num_rows = num_rows,
            .num_cols = num_cols,
            .cells = ArrayList.init(CellType, allocator)
        };
    }

    pub fn init_with_array(num_rows: usize, num_cols: usize, cells: std.ArrayList(CellType)) Map2D {
        return Map2D {
            .num_rows = num_rows,
            .num_cols = num_cols,
            .cells = cells
        };
    }

    pub fn n_rows(self: Map2D) usize {
        return self.num_rows;
    }

    pub fn n_cols(self: Map2D) usize {
        return self.num_cols;
    }

    pub fn at(self: Map2D, row: usize, col: usize) ?CellType {
        const index = self.index_of(row, col) orelse return null;
        return self.cells.items[index];
    }

    pub fn at_mut(self: Map2D, row: usize, col: usize) ?*CellType {
        const index = self.index_of(row, col) orelse return null;
        return &self.cells.items[index];
    }


    pub fn has_same_state(self: Map2D, other: Map2D) bool {
        if (self.num_rows != other.num_rows or self.num_cols != other.num_cols) {
            return false;
        }
        var i: usize = 0;
        while (i < self.num_rows): (i += 1)
        {
            var j: usize = 0;
            while (j < self.num_cols): (j += 1) {
                if (self.at(i, j) orelse unreachable != other.at(i, j) orelse unreachable) {
                    return false;
                }
            }
        }
        return true;
    }

    pub fn print(self: Map2D) void {
        var i: usize = 0;
        while (i < self.cells.items.len): (i += 1)
        {
            if ((i % self.num_cols) == 0) {
                print("\n", .{});
            }
            const symbol = switch (self.cells.items[i]) {
                CellType.Floor => ".",
                CellType.OccupiedSeat => "#",
                CellType.EmptySeat => "L",
            };
            print("{}", .{symbol});
        }
        print("\n", .{});
    }


    pub fn count_cells_with_value(self: Map2D, value: CellType) u32 {
        var count: u32 = 0;
        var i: usize = 0;
        while (i < self.cells.items.len): (i += 1)
        {
            if (self.cells.items[i] == value) {
                count += 1;
            }
        }
        return count;
    }


    fn index_of(self: Map2D, row: usize, col: usize) ?usize {
        if (row >= self.num_rows or col >= self.num_cols) {
            return null;
        }
        const index = (self.num_cols * row) + col;
        std.debug.assert(index < self.cells.items.len);
        return index;
    }

};

fn count_ajacent_occupied_seat(map: Map2D, r: usize, c: usize) u32 {
    var count: u32 = 0;

    var i: usize = if (r == 0) r else r - 1;
    var visited: usize = 0;
    while (i < r + 2): (i += 1)
    {
        var j: usize = if (c == 0) 0 else c - 1;
        while (j < c + 2): (j += 1) {
            if (i == r and j == c) {
                continue;
            }

            if (map.at(i, j)) |cell_type| {
                if (cell_type == CellType.OccupiedSeat) {
                    count += 1;
                }
                visited += 1;
            }
        }
    }
    return count;
}


fn do_one_round(previous_map: Map2D, current_map: Map2D) void {
    var i: usize = 0;
    while (i < previous_map.n_rows()): (i += 1)
    {
        var j: usize = 0;
        while (j < previous_map.n_cols()): (j += 1) {
            const cell_type = previous_map.at(i, j) orelse @panic("invalid iteration ?");
            if (previous_map.at(i, j) == CellType.Floor) {
                continue;
            }

            const num_occupied = count_ajacent_occupied_seat(previous_map, i, j);
            // Doing i( map.at(i, j) == CellType.whatever {} results in a compiler error 
            // "instruction does not dominate all use
            if (cell_type == CellType.OccupiedSeat and num_occupied >= 4)
            {
                var val: *CellType = current_map.at_mut(i, j) orelse @panic("fack");
                val.* = CellType.EmptySeat;
            } else if (cell_type == CellType.EmptySeat and num_occupied == 0)
            {
                var val = current_map.at_mut(i, j) orelse @panic("fack");
                val.* = CellType.OccupiedSeat;
            }
        }
    }
}

pub fn main() !void {

    var allocator = std.heap.GeneralPurposeAllocator(.{}){};

    var input_file = try fs.openFileAbsoluteZ("C:/Users/t.montaigu/Projects/learn-zig/day_11.txt", fs.File.OpenFlags{.read = true});
    defer input_file.close();

    var line_reader = LineReader.init(input_file);
    var cells = std.ArrayList(CellType).init(&allocator.allocator);

    var num_rows: usize = 0;
    var num_cols: usize = 0;
    while (try line_reader.next_line()) | line | {
        print("line: '{}'\n", .{line});
        var cols: usize = 0;
        for (line)  |byte| {
            if (CellType.from_byte(byte)) |t| {
                try cells.append(t);
            } else {
                print("{} is not a valud symbol for the map\n", .{byte});
                break;
            }
            cols += 1;
        }
        num_rows += 1;
        num_cols = cols;
    }

    var current_map = Map2D.init_with_array(num_rows, num_cols, cells);


    var previous_cells = std.ArrayList(CellType).init(&allocator.allocator);
    for (current_map.cells.items) |value| {
        try previous_cells.append(value);
    }

    var previous_map = Map2D.init_with_array(num_rows, num_cols, previous_cells);

    while (true) {
        var i: usize = 0;
        while (i < current_map.cells.items.len): (i += 1) {
            previous_map.cells.items[i] = current_map.cells.items[i];
        }
        do_one_round(previous_map, current_map);

        if (current_map.has_same_state(previous_map)) {
            break;
        }

    }
    print("occupied seats: {}\n", .{current_map.count_cells_with_value(CellType.OccupiedSeat)});
}
