import java.io.File;
import java.util.Collections;
import java.util.LinkedList;
import java.util.Scanner;

public class schedSim {
    private static LinkedList<Job> jobs = new LinkedList<>();

    public static void main(String[] args) {
        parseArgs(args);
    }

    static void parseArgs(String[] args) {
        if (args.length == 0) {
            System.out.println("Format should be: schedSim <job-file.txt> -p <ALGORITHM> -q <QUANTUM>");
            System.exit(0);
        }

        parseFile(args[0]);

        if (args[1].equals("-p")) {
            checkArgs(args, 2, 3);
        } else if (args[3].equals("-p")) {
            checkArgs(args, 4, 1);
        }

      //  System.out.println(jobs.toString());
    }

    static void checkArgs(String[] args, int p_index, int q_index) {
        switch (args[p_index]) {
            case "SRJN":
                srjn();
                break;
            case "FIFO":
                fifo();
                break;
            case "RR":
                if (args.length > 3 && args[q_index].equals("-q")) {
                    rr(Integer.valueOf(args[q_index + 1]));
                } else {
                    rr(1);
                }
                break;
            default:
                fifo();
                break;
        }
        turnaround_time();
        printJobs();
        printAverage();
    }

    static void parseFile(String f) {
        File file = new File(f);
        int burst, arrival;
        int i = 0;

        try {
            Scanner scanner = new Scanner(file);
            while (scanner.hasNext()) {
                burst = scanner.nextInt();
                arrival = scanner.nextInt();
                jobs.add(new Job(burst, arrival));
                Collections.sort(jobs);
            }
            for (Job j : jobs) {
                j.number = i;
                i++;
            }
        } catch (Exception e) {
            System.out.println(e);
        }
    }

    static void srjn() {
        int totalTime = 0, completeJobs = 0, minmum_burst = Integer.MAX_VALUE;
        int shortestJob = 0, finish;
        boolean changed = false;

        while (completeJobs != jobs.size()) {
            for (Job j : jobs) {
                if (j.arrival <= totalTime && j.remaining_time < minmum_burst && j.remaining_time > 0) {
                    minmum_burst = j.burst;
                    shortestJob = j.number;
                    changed = true;
                }
            }

            if (!changed) {
                totalTime++;
            }

            minmum_burst = --jobs.get(shortestJob).remaining_time;

            if (minmum_burst == 0) {
                minmum_burst = Integer.MAX_VALUE;
                completeJobs++;
                finish = totalTime + 1;
                jobs.get(shortestJob).wait = finish - jobs.get(shortestJob).burst - jobs.get(shortestJob).arrival;
            }
            totalTime++;
        }
    }

    static void rr(int q) {
        int totalTime = 0;
        while (true) {
            boolean done = true;

            for (Job j : jobs) {
                if (j.remaining_time > 0) {
                    done = false;
                    if (j.remaining_time > q) {
                        totalTime += q;
                        j.remaining_time -= q;
                    } else {
                        totalTime += j.remaining_time;
                        j.wait = totalTime - j.burst - j.arrival;
                        j.remaining_time = 0;
                    }
                }
            }

            if (done) {
                break;
            }
        }
    }

    static void turnaround_time() {
        for (Job j : jobs) {
            j.turnaround = j.wait + j.burst;
        }
    }

    static void fifo() {
        int totalTime = 0;

        for (Job j : jobs) {
            j.wait = totalTime - j.arrival;
            totalTime += j.burst;
        }
    }

    static void printJobs() {
        for (Job j : jobs) {
            System.out.format("Job %3d -- Turnaround %3.2f  Wait %3.2f\n", j.number, (float)j.turnaround, (float)j.wait);
        }
    }

    static void printAverage() {
        float total_Turnaround = 0, total_wait = 0;

        for (Job j : jobs) {
            total_Turnaround += j.turnaround;
            total_wait += j.wait;
        }

        System.out.format("Average -- Turnaround %3.2f  Wait %3.2f\n", total_Turnaround / jobs.size(), total_wait / jobs.size());
    }

    private static class Job implements Comparable<Job> {
        int burst;
        int arrival;
        int number;
        int turnaround;
        int wait;
        int remaining_time;

        Job(int burst, int arrival) {
            this.arrival = arrival;
            this.burst = burst;
            this.remaining_time = burst;
        }

        @Override
        public int compareTo(Job j) {
            return this.arrival - j.arrival;
        }

        @Override
        public String toString() {
            return "arrival: " + arrival + " burst: " + burst + " number: " + number + "\n";
        }
    }
}
