import puz
import json

p = puz.load(open('test.puz', 'rb').read())

print(p.solution)
print(p.width, p.height)
print(p.clues)

print(f'''    const char *clues[] = {{
{
''.join(f'        {json.dumps(clue)},{chr(10)}' for clue in p.clues)}
    }};
    struct puzzle *puzzle = puzzle_new({p.width}, {p.height}, "{p.solution}", clues);
''')
