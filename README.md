# DystroCPLEX
To repozytorium implementuje w narzedziu CPLEX (w C++ z wykorzystaniem Concert API) problem "Distribution 1" z ksiazki Williamsa (rozdzialy 12.19, 13.19, 14.19).

W folderze opl mozna znalezc takze rozwiazanie zaimplementowane w jezyku OPL z ktorego korzysta CPLEX IDE
# Pobieranie
```git clone https://github.com/Kacper79/DystroCPLEX.git```

```cd DystroCPLEX```

# Budowanie
Wazne jest aby w pliku CMakeLists.txt ustawic sobie prawidlowa sciezke do folderu gdzie zostal zainstalowany CPLEX na komputerze lokalnym

```set(CPLEX_ROOT_PATH "nowa_sciezka_do_folderu_cplex") ```
gdzie sciezka (na Windowsie) musi byc podawana ze slashami zwyklymi (/) a nie odwrotnymi (\\) ktorych CMake nie lubi

Jezeli wersje CPLEX sie roznia, trzeba zmienic nazwe biblioteki w pliku CMakeLists przy podpinaniu bibliotek (.dll) np. zmienic na starsza wersje z cplex2212 na cplex2211 (jak w pracowni UKSW)

```cmake -S . -B build```

```cmake --build build --config Release```
