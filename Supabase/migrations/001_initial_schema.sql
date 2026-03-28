-- Liminal Spaces - Initial Database Schema
-- Run this in your Supabase SQL Editor

-- Player profiles (extends Supabase auth.users)
CREATE TABLE IF NOT EXISTS public.player_profiles (
    id UUID PRIMARY KEY REFERENCES auth.users(id) ON DELETE CASCADE,
    display_name TEXT NOT NULL DEFAULT 'Explorer',
    avatar_url TEXT,
    maps_completed INTEGER DEFAULT 0,
    enemies_defeated INTEGER DEFAULT 0,
    total_play_time_seconds FLOAT DEFAULT 0,
    created_at TIMESTAMPTZ DEFAULT NOW(),
    updated_at TIMESTAMPTZ DEFAULT NOW()
);

-- Enable RLS
ALTER TABLE public.player_profiles ENABLE ROW LEVEL SECURITY;

-- Players can read their own profile
CREATE POLICY "Players can view own profile"
    ON public.player_profiles FOR SELECT
    USING (auth.uid() = id);

-- Players can update their own profile
CREATE POLICY "Players can update own profile"
    ON public.player_profiles FOR UPDATE
    USING (auth.uid() = id);

-- Players can insert their own profile
CREATE POLICY "Players can insert own profile"
    ON public.player_profiles FOR INSERT
    WITH CHECK (auth.uid() = id);

-- All players can view display names (for leaderboards)
CREATE POLICY "Anyone can view display names"
    ON public.player_profiles FOR SELECT
    USING (true);

-- Auto-create profile on signup
CREATE OR REPLACE FUNCTION public.handle_new_user()
RETURNS TRIGGER AS $$
BEGIN
    INSERT INTO public.player_profiles (id, display_name)
    VALUES (NEW.id, COALESCE(NEW.raw_user_meta_data->>'display_name', 'Explorer'));
    RETURN NEW;
END;
$$ LANGUAGE plpgsql SECURITY DEFINER;

CREATE OR REPLACE TRIGGER on_auth_user_created
    AFTER INSERT ON auth.users
    FOR EACH ROW EXECUTE FUNCTION public.handle_new_user();

-- Map completions / Leaderboard
CREATE TABLE IF NOT EXISTS public.map_completions (
    id BIGSERIAL PRIMARY KEY,
    player_id UUID NOT NULL REFERENCES public.player_profiles(id) ON DELETE CASCADE,
    map_id TEXT NOT NULL,
    completion_time_seconds FLOAT NOT NULL,
    enemies_defeated INTEGER DEFAULT 0,
    difficulty TEXT DEFAULT 'Medium',
    completed_at TIMESTAMPTZ DEFAULT NOW()
);

ALTER TABLE public.map_completions ENABLE ROW LEVEL SECURITY;

-- Anyone can read the leaderboard
CREATE POLICY "Anyone can view leaderboard"
    ON public.map_completions FOR SELECT
    USING (true);

-- Players can insert their own completions
CREATE POLICY "Players can insert own completions"
    ON public.map_completions FOR INSERT
    WITH CHECK (auth.uid() = player_id);

-- Indexes for leaderboard queries
CREATE INDEX idx_map_completions_map_time
    ON public.map_completions (map_id, completion_time_seconds ASC);

CREATE INDEX idx_map_completions_player
    ON public.map_completions (player_id);

-- Player saved game data (cloud saves)
CREATE TABLE IF NOT EXISTS public.player_saves (
    id BIGSERIAL PRIMARY KEY,
    player_id UUID NOT NULL REFERENCES public.player_profiles(id) ON DELETE CASCADE,
    slot_name TEXT NOT NULL DEFAULT 'default',
    save_data JSONB NOT NULL DEFAULT '{}',
    saved_at TIMESTAMPTZ DEFAULT NOW(),
    UNIQUE (player_id, slot_name)
);

ALTER TABLE public.player_saves ENABLE ROW LEVEL SECURITY;

CREATE POLICY "Players can manage own saves"
    ON public.player_saves FOR ALL
    USING (auth.uid() = player_id);

-- Updated_at trigger function
CREATE OR REPLACE FUNCTION public.update_updated_at()
RETURNS TRIGGER AS $$
BEGIN
    NEW.updated_at = NOW();
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE TRIGGER update_player_profiles_updated_at
    BEFORE UPDATE ON public.player_profiles
    FOR EACH ROW EXECUTE FUNCTION public.update_updated_at();

-- Leaderboard view (top 100 per map, with display names)
CREATE OR REPLACE VIEW public.leaderboard AS
SELECT
    mc.id,
    mc.map_id,
    mc.completion_time_seconds,
    mc.enemies_defeated,
    mc.difficulty,
    mc.completed_at,
    pp.display_name,
    pp.avatar_url,
    ROW_NUMBER() OVER (PARTITION BY mc.map_id ORDER BY mc.completion_time_seconds ASC) as rank
FROM public.map_completions mc
JOIN public.player_profiles pp ON mc.player_id = pp.id;
