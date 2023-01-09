using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace GrIso
{
    class GraphHarderIsoTests
    {
        public static void TestHarderIso()
        {
            /*
            var graph_harder_iso = new GraphHarderIso
            {
                TestMode = true,
                GraphsCount = 5,
                VerticesCount = 10,
                EdgesCount = 20,
                CountNotImproved = 10,
                MutationsLimit = 100
            };
            //graph_harder_iso.StatusHandlers += x => { };
            graph_harder_iso.TryHarderIso();
            */

            var graph_harder_iso = new GraphHarderIso
            {
                TestMode = true,
                GraphsCount = 10,
                VerticesCount = 20,
                EdgesCount = 50,
                CountNotImproved = 10,
                MutationsLimit = 100
            };            
            graph_harder_iso.TryHarderIso();
        }

        public static void RunHarderIso()
        {
            DisplayKeys();

            var graph_harder_iso = new GraphHarderIso
            {
                GraphsCount = 100,
                VerticesCount = 100,
                EdgesCount = 500,
                RankNotImproved = 5,
                CountNotImproved = 10,
            };
            graph_harder_iso.StatusHandlers += HandleStatus;
            graph_harder_iso.TryHarderIso();

            DisplayStatus(graph_harder_iso);
            Console.ReadKey();
        }

        public static void RunRandomIso()
        {
            DisplayKeys();

            var graph_harder_iso = new GraphRandomIso
            {
                VerticesCount = 100,
                EdgesCount = 500,
                SubVerticesCount = 0,
                SubEdgesCount = 0
            };
            graph_harder_iso.StatusHandlers += HandleStatus;
            graph_harder_iso.TryRandomIso();

            DisplayStatus(graph_harder_iso);
            Console.ReadKey();
        }

        static void HandleStatus(GraphHarderIso graph_harder_iso)
        {
            if (Console.KeyAvailable)
            {
                var read_key = Console.ReadKey();
                if (read_key.Key == ConsoleKey.Q)
                    graph_harder_iso.Satisfacted = true;
                else if (read_key.Key == ConsoleKey.P)
                    DisplayParams(graph_harder_iso);
                else if (read_key.Key == ConsoleKey.S)
                    DisplayStatus(graph_harder_iso);
                else
                    DisplayKeys();
            }
        }

        static void DisplayKeys()
        {
            Console.WriteLine("Q quit P parameters S status");
        }

        static void DisplayStatus(GraphHarderIso graph_harder_iso)
        {
            Console.WriteLine(
                $@"mutations {graph_harder_iso.MutationsCount} improved {graph_harder_iso.ImprovedCount} removed {graph_harder_iso.RemovedCount}
                initial cost {graph_harder_iso.InitialIsoCost} {graph_harder_iso.InitialTimeCost} 
               current cost {graph_harder_iso.CurrentIsoCost} {graph_harder_iso.CurrentTimeCost} 
            ");
        }

        static void DisplayParams(GraphHarderIso graph_harder_iso)
        {
            Console.WriteLine($@"
                   graphs {graph_harder_iso.GraphsCount} vertices {graph_harder_iso.VerticesCount} edges {graph_harder_iso.EdgesCount}
                   not improved rank and count {graph_harder_iso.RankNotImproved} {graph_harder_iso.CountNotImproved}
            ");
        }

        static void HandleStatus(GraphRandomIso graph_harder_iso)
        {
            if (Console.KeyAvailable)
            {
                var read_key = Console.ReadKey();
                if (read_key.Key == ConsoleKey.Q)
                    graph_harder_iso.Satisfacted = true;
                else if (read_key.Key == ConsoleKey.P)
                    DisplayParams(graph_harder_iso);
                else if (read_key.Key == ConsoleKey.S)
                    DisplayStatus(graph_harder_iso);
                else
                    DisplayKeys();
            }
        }

        static void DisplayStatus(GraphRandomIso graph_harder_iso)
        {
            Console.WriteLine(
                $@"random count {graph_harder_iso.RandomCount} improved count {graph_harder_iso.ImprovedCount}
                initial cost {graph_harder_iso.InitialIsoCost} {graph_harder_iso.InitialTimeCost} 
               current cost {graph_harder_iso.CurrentIsoCost} {graph_harder_iso.CurrentTimeCost} 
            ");
        }

        static void DisplayParams(GraphRandomIso graph_harder_iso)
        {
            Console.WriteLine($@"
                   graph vertices {graph_harder_iso.VerticesCount} edges {graph_harder_iso.EdgesCount} subgraph vertices {graph_harder_iso.SubVerticesCount} edges {graph_harder_iso.SubEdgesCount} 
            ");
        }
    }
}
