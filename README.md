# GameplayTag Filter (Unreal Engine)

Пять Blueprint-нод, которые достают акторы с нужным Gameplay Tag из **массива** или **мапы** одной нодой — вместо связки ForEach → Actor Has Tag → Branch → Add.

**[🇬🇧 English version →](README_EN.md)**

## Совместимость

- Unreal Engine **5.2 и 5.8** (Win64), исходники без версионной специфики
- Один Runtime-модуль, GAS (AbilitySystemComponent) **не нужен** — используются стандартные Actor Tags (FName)

## Установка

Скопируйте папку `GameplayTagFilter` в `<Проект>/Plugins/` и перезапустите редактор.
Ноды появятся в палитре в категории **Tag Filter**.

## Ноды

| Нода | Вход | Выход |
|---|---|---|
| **Filter Actors by Tag** | массив акторов + один тег | совпавшие акторы + `MatchCount` |
| **Filter Actors by Tags** | массив акторов + контейнер тегов + режим **Any/All** | совпавшие акторы + `MatchCount` |
| **Find Actors in Tag Map** | `TMap<FGameplayTag, AActor*>` + тег (сравниваются **ключи**) | акторы + совпавшие ключи + `MatchCount` |
| **Find Actors in Tag Groups** | массив `FTaggedActorArray {Tag, Actors[]}` + тег | акторы всех подошедших групп + их теги + `MatchCount` |
| **Filter Actors Map by Tag** | `TMap<AActor*, FGameplayTagContainer>` + тег (сравниваются **теги-значения**) | акторы, чьи теги совпали + `MatchCount` |

Все ноды **BlueprintPure** — без exec-пинов.

## Сравнение тегов (пин `bExactMatch`)

- **false** (по умолчанию) — иерархическое сравнение, как в GAS: запрос `Damage` находит `Damage` и `Damage.Fire`, но не родителя (запрос `Damage.Fire` не найдёт актора с тегом `Damage`).
- **true** — только точное совпадение имени.

В мапе `Тег → Актoр` поиск идёт по **ключу**: запрос `Damage` вытащит и запись с ключом `Damage.Fire`.
Иерархия работает для тегов, зарегистрированных в словаре проекта (**Project Settings → Gameplay Tags**); незарегистрированные имена матчатся только точно.

## Пример

```text
Массив акторов:  Костёр [Damage.Fire]   Лук [Damage]   Дверь [Door]

Filter Actors by Tag (запрос: Damage, bExactMatch = false)
   → Костёр, Лук          (MatchCount = 2)

Filter Actors by Tag (запрос: Damage, bExactMatch = true)
   → Лук                  (MatchCount = 1)
```

## Почему «тег → массив акторов» — это массив структур

Blueprint запрещает `TMap<FGameplayTag, TArray<AActor*>>` (вложенные контейнеры в мапах запрещены движком), поэтому вариант «тег → группа акторов» реализован нодой **Find Actors in Tag Groups**, принимающей массив структуры `FTaggedActorArray` (`Tag` + `Actors`) — семантика та же.

## Сборка из исходников

Через UAT (автономная сборка плагина):

```bat
"<Engine>\Engine\Build\BatchFiles\RunUAT.bat" BuildPlugin ^
  "-Plugin=<Path>\GameplayTagFilter.uplugin" "-Package=<OutDir>" -TargetPlatforms=Win64 -NoSubmit
```

Либо как часть проекта: скопируйте папку плагина в `<Проект>/Plugins/` и соберите таргет проекта (`Build.bat <Проект>Editor Win64 Development -project="<Проект>.uproject"`).

> Примечание: в UE 5.x модуль должен содержать явный `IMPLEMENT_MODULE(FDefaultModuleImpl, GameplayTagFilter);` — UHT не всегда генерирует обвязку сам (это уже учтено в исходниках).
