// DystroCPLEX.cpp : Defines the entry point for the application.
//

#include <DystroCPLEX.h>

ILOSTLBEGIN

struct ArcFD {
    int f;          // factory index
    int d;          // depot index
    double cost;
};

struct ArcFC {
    int f;          // factory index
    int c;          // customer index
    double cost;
};

struct ArcDC {
    int d;          // depot index
    int c;          // customer index
    double cost;
};

int main() {
    IloEnv env;

    try {
        IloModel model(env);

        // =========================
        // Sets / names
        // =========================
        const int NF = 2;
        const int ND = 4;
        const int NC = 6;

        std::vector<std::string> factories = { "Liverpool", "Brighton" };
        std::vector<std::string> depots = { "Newcastle", "Birmingham", "London", "Exeter" };
        std::vector<std::string> customers = { "C1", "C2", "C3", "C4", "C5", "C6" };

        // =========================
        // Data (in thousand tons)
        // =========================
        double capF[NF] = { 150.0, 200.0 };
        double capD[ND] = { 70.0, 50.0, 100.0, 40.0 };
        double dem[NC] = { 50.0, 10.0, 40.0, 35.0, 60.0, 20.0 };

        // Allowed arcs with costs
        std::vector<ArcFD> arcsFD = {
            {0, 0, 0.5},  // Liverpool -> Newcastle
            {0, 1, 0.5},  // Liverpool -> Birmingham
            {0, 2, 1.0},  // Liverpool -> London
            {0, 3, 0.2},  // Liverpool -> Exeter
            {1, 1, 0.3},  // Brighton -> Birmingham
            {1, 2, 0.5},  // Brighton -> London
            {1, 3, 0.2}   // Brighton -> Exeter
        };

        std::vector<ArcFC> arcsFC = {
            {0, 0, 1.0},  // Liverpool -> C1
            {1, 0, 2.0},  // Brighton  -> C1
            {0, 2, 1.5},  // Liverpool -> C3
            {0, 3, 2.0},  // Liverpool -> C4
            {0, 5, 1.0}   // Liverpool -> C6
        };

        std::vector<ArcDC> arcsDC = {
            {0, 1, 1.5},  // Newcastle  -> C2
            {1, 1, 0.5},  // Birmingham -> C2
            {2, 1, 1.5},  // London     -> C2

            {0, 2, 0.5},  // Newcastle  -> C3
            {1, 2, 0.5},  // Birmingham -> C3
            {2, 2, 2.0},  // London     -> C3
            {3, 2, 0.2},  // Exeter     -> C3

            {0, 3, 1.5},  // Newcastle  -> C4
            {1, 3, 1.0},  // Birmingham -> C4
            {3, 3, 1.5},  // Exeter     -> C4

            {1, 4, 0.5},  // Birmingham -> C5
            {2, 4, 0.5},  // London     -> C5
            {3, 4, 0.5},  // Exeter     -> C5

            {0, 5, 1.0},  // Newcastle  -> C6
            {2, 5, 1.5},  // London     -> C6
            {3, 5, 1.5}   // Exeter     -> C6
        };

        // =========================
        // Decision variables
        // =========================
        IloNumVarArray x(env);   // factory -> depot
        IloNumVarArray y(env);   // factory -> customer
        IloNumVarArray z(env);   // depot   -> customer

        for (std::size_t a = 0; a < arcsFD.size(); ++a)
            x.add(IloNumVar(env, 0.0, IloInfinity, ILOFLOAT));

        for (std::size_t a = 0; a < arcsFC.size(); ++a)
            y.add(IloNumVar(env, 0.0, IloInfinity, ILOFLOAT));

        for (std::size_t a = 0; a < arcsDC.size(); ++a)
            z.add(IloNumVar(env, 0.0, IloInfinity, ILOFLOAT));

        // =========================
        // Objective: minimize cost
        // =========================
        IloExpr obj(env);

        for (std::size_t a = 0; a < arcsFD.size(); ++a)
            obj += arcsFD[a].cost * x[(IloInt)a];

        for (std::size_t a = 0; a < arcsFC.size(); ++a)
            obj += arcsFC[a].cost * y[(IloInt)a];

        for (std::size_t a = 0; a < arcsDC.size(); ++a)
            obj += arcsDC[a].cost * z[(IloInt)a];

        model.add(IloMinimize(env, obj));
        obj.end();

        // =========================
        // Constraints
        // =========================

        // 1. Factory capacities
        for (int f = 0; f < NF; ++f) {
            IloExpr expr(env);

            for (std::size_t a = 0; a < arcsFD.size(); ++a)
                if (arcsFD[a].f == f)
                    expr += x[(IloInt)a];

            for (std::size_t a = 0; a < arcsFC.size(); ++a)
                if (arcsFC[a].f == f)
                    expr += y[(IloInt)a];

            model.add(expr <= capF[f]);
            expr.end();
        }

        // 2. Depot inbound capacity
        for (int d = 0; d < ND; ++d) {
            IloExpr expr(env);

            for (std::size_t a = 0; a < arcsFD.size(); ++a)
                if (arcsFD[a].d == d)
                    expr += x[(IloInt)a];

            model.add(expr <= capD[d]);
            expr.end();
        }

        // 3. Depot flow balance
        for (int d = 0; d < ND; ++d) {
            IloExpr inflow(env);
            IloExpr outflow(env);

            for (std::size_t a = 0; a < arcsFD.size(); ++a)
                if (arcsFD[a].d == d)
                    inflow += x[(IloInt)a];

            for (std::size_t a = 0; a < arcsDC.size(); ++a)
                if (arcsDC[a].d == d)
                    outflow += z[(IloInt)a];

            model.add(outflow == inflow);

            inflow.end();
            outflow.end();
        }

        // 4. Customer demand satisfaction
        for (int c = 0; c < NC; ++c) {
            IloExpr expr(env);

            for (std::size_t a = 0; a < arcsFC.size(); ++a)
                if (arcsFC[a].c == c)
                    expr += y[(IloInt)a];

            for (std::size_t a = 0; a < arcsDC.size(); ++a)
                if (arcsDC[a].c == c)
                    expr += z[(IloInt)a];

            model.add(expr == dem[c]);
            expr.end();
        }

        // =========================
        // Solve
        // =========================
        IloCplex cplex(model);

        // opcjonalnie: wyłącz log
         cplex.setOut(env.getNullStream());

        if (!cplex.solve()) {
            std::cout << "Brak rozwiazania.\n";
            env.end();
            return 1;
        }

        std::cout << std::fixed << std::setprecision(3);
        std::cout << "Status: " << cplex.getStatus() << "\n";
        std::cout << "Minimalny koszt = " << cplex.getObjValue()
            << " (tys. GBP / miesiac, bo przeplywy sa w tys. ton)\n\n";

        std::cout << "Fabryka -> Depot\n";
        for (std::size_t a = 0; a < arcsFD.size(); ++a) {
            double val = cplex.getValue(x[(IloInt)a]);
            if (val > 1e-6) {
                std::cout << factories[arcsFD[a].f] << " -> "
                    << depots[arcsFD[a].d] << " : "
                    << val << "\n";
            }
        }

        std::cout << "\nFabryka -> Klient\n";
        for (std::size_t a = 0; a < arcsFC.size(); ++a) {
            double val = cplex.getValue(y[(IloInt)a]);
            if (val > 1e-6) {
                std::cout << factories[arcsFC[a].f] << " -> "
                    << customers[arcsFC[a].c] << " : "
                    << val << "\n";
            }
        }

        std::cout << "\nDepot -> Klient\n";
        for (std::size_t a = 0; a < arcsDC.size(); ++a) {
            double val = cplex.getValue(z[(IloInt)a]);
            if (val > 1e-6) {
                std::cout << depots[arcsDC[a].d] << " -> "
                    << customers[arcsDC[a].c] << " : "
                    << val << "\n";
            }
        }

        env.end();
    }
    catch (IloException& e) {
        std::cerr << "CPLEX exception: " << e << "\n";
        env.end();
        return 2;
    }
    catch (...) {
        std::cerr << "Unknown exception.\n";
        env.end();
        return 3;
    }

    return 0;
}