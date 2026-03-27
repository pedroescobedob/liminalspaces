# Supabase Setup for Liminal Spaces

## Setup Steps

1. Go to your Supabase project dashboard
2. Open the SQL Editor
3. Run `migrations/001_initial_schema.sql`
4. Copy your project URL and anon key from Settings > API
5. Paste them into `Config/DefaultGame.ini` under `[/Script/LiminalSpaces.LSSupabaseSubsystem]`

## Tables

| Table | Purpose |
|-------|---------|
| `player_profiles` | Player display name, stats (auto-created on signup) |
| `map_completions` | Leaderboard entries per map |
| `player_saves` | Cloud save data (JSON) |

## Views

| View | Purpose |
|------|---------|
| `leaderboard` | Ranked map completions with display names |

## Row Level Security

All tables have RLS enabled:
- Players can only modify their own data
- Leaderboard is publicly readable
- Profile display names are publicly readable

## API Endpoints Used by the Game

- `POST /auth/v1/signup` - Register
- `POST /auth/v1/token?grant_type=password` - Login
- `POST /auth/v1/token?grant_type=refresh_token` - Refresh session
- `POST /rest/v1/map_completions` - Submit score
- `GET /rest/v1/leaderboard?map_id=eq.{id}&order=completion_time_seconds.asc` - Get leaderboard
- `POST /rest/v1/player_saves` - Save game
- `GET /rest/v1/player_saves?player_id=eq.{id}` - Load game
