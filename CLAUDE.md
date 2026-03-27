# Liminal Spaces - Development Guide

## Project Overview
First-person exploration horror game built with Unreal Engine 5.7 (C++ + Blueprint hybrid).

## Build System
- **Engine**: UE 5.7 at `/Users/Shared/Epic Games/UE_5.7`
- **Module**: `LiminalSpaces` (Runtime, Default loading phase)
- **Target files**: `Source/liminalspaces.Target.cs`, `Source/liminalspacesEditor.Target.cs`
- **Build rules**: `Source/LiminalSpaces/LiminalSpaces.Build.cs`

## Code Conventions
- All C++ classes prefixed with `LS` (e.g., `ALSCharacter`, `ULSHealthComponent`)
- UPROPERTY categories use `"LiminalSpaces|SubCategory"` format
- Components use composition pattern (HealthComponent, CombatComponent, InteractionComponent)
- Data-driven design via `UPrimaryDataAsset` subclasses (MapDefinition, EnemyDefinition, WeaponDefinition)
- Game uses Enhanced Input System (not legacy input)
- AI uses Perception (sight + hearing) with simple state-based behavior (future: StateTree)

## Source Directory Layout
```
Source/LiminalSpaces/
  Core/     - GameMode, GameState, PlayerController, PlayerState, GameInstance, SaveGame
  Player/   - Character, InteractionComponent
  Combat/   - HealthComponent, CombatComponent, WeaponBase, Projectile
  Enemy/    - EnemyBase, EnemyAIController, EnemySpawnPoint
  Level/    - BedCellActor, EscapePoint, DoorActor, IInteractable
  UI/       - HUD, HUDWidget, MainMenuWidget, PauseMenuWidget, MapSelectWidget
  Audio/    - AudioManager, AmbientZone
  Online/   - SupabaseSubsystem
  Data/     - Types, MapDefinition, EnemyDefinition, WeaponDefinition
```

## Content Organization
- `Content/LiminalSpaces/` - All game-specific content
- `Content/FabAssets/` - Imported Fab marketplace assets (organized by category)
- `Content/FirstPerson/` - Original template (reference only)

## Key Config Files
- `Config/DefaultEngine.ini` - Rendering, collision, game maps, module redirects (lines 255-256)
- `Config/DefaultGame.ini` - Project name, asset manager, Supabase config
- `Config/DefaultInput.ini` - Enhanced Input configuration

## Backend
- Supabase URL/Key configured in `DefaultGame.ini` under `[/Script/LiminalSpaces.LSSupabaseSubsystem]`
- Never commit service_role key

## Git
- Remote: https://github.com/pedroescobedob/liminalspaces.git
- Large files (*.MOV) should eventually use Git LFS
