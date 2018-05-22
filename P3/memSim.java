import java.io.File;
import java.io.RandomAccessFile;
import java.util.*;

public class memSim {
    static PageEntry[] pageTable = new PageEntry[256];
    static PriorityQueue<TLBEntry> tlb = new PriorityQueue<>();
    static LinkedList<Reference> references = new LinkedList<>();
    static LinkedList<PhysicalMemoryEntry> physicalMemory = new LinkedList<>();
    static int frameNbrs = 256;
    static final int FIFO = 0;
    static final int LRU = 1;
    static final int OPT = 2;
    static int pageFaults = 0;
    static int tlbHit = 0;
    static int tlbMisses = 0;
    static String textFile = "";
    static int replacement = 0;
    static Reference opt = null;

    public static void main(String[] args) {
        parseArgs(args);
        parseFile(textFile);
        checkReferences();
        System.out.println("Number of Translated Addresses = " + references.size());
        System.out.println("Page Faults = " + pageFaults);
        System.out.format("Page Fault Rate = %1.3f\n", 1.0 * pageFaults / references.size());
        System.out.println("TLB Hits = " + tlbHit);
        System.out.println("TLB Misses = " + tlbMisses);
        System.out.format("TLB Hit Rate = %1.3f\n", 1.0 * tlbHit / references.size());
    }

    private static void parseArgs(String[] args) {
        try {
            if (args[0] != null) {
                textFile = args[0];
            }
            if (args[1] != null) {
                try {
                    frameNbrs = Integer.parseInt(args[1]);
                } catch (Exception e) {
                    if (args[1].equals("LRU")) {
                        replacement = LRU;
                    } else if (args[1].equals("OPT")) {
                        replacement = OPT;
                    }
                }
            }
            if (args[2] != null) {
                if (args[2].equals("LRU")) {
                    replacement = LRU;
                } else if (args[2].equals("OPT")) {
                    replacement = OPT;
                }
            }
        } catch (Exception ignored) {

        }
    }

    private static void parseFile(String filename) {
        try {
            File f = new File(filename);
            Scanner s = new Scanner(f);
            int current;
            while (s.hasNext()) {
                current = s.nextInt();
                byte offset = (byte) (current & 0xFF);
                byte page = (byte) (current >> 8 & 0xFF);
                references.add(new Reference(page, offset, current));
            }
        } catch (Exception e) {
            System.out.println(e.toString());
        }
    }

    private static void checkReferences() {
        byte[] read;
        int frameNbr;

        for (Reference r : references) {
            opt = r;
            frameNbr = checkTLB(r.getPageNbrVal());
            if (frameNbr == -1) {
                tlbMisses++;
                frameNbr = checkPageTable(r.getPageNbrVal());
                if (frameNbr == -1) {
                    pageFaults++;
                    read = getFromFile(r.getPageNbrVal());
                    frameNbr = addToPhysicalMemory(read);
                    addToPageTable(frameNbr, r.getPageNbrVal());
                    updateTLB(frameNbr, r.getPageNbrVal());
                } else {
                    if (replacement == LRU || replacement == OPT) {
                        incrementPhysicalEntryCounts();
                        physicalMemory.get(frameNbr).resetCount();
                    }
                }
            } else {
                if (replacement == LRU || replacement == OPT) {
                    incrementPhysicalEntryCounts();
                    physicalMemory.get(frameNbr).resetCount();
                }
                tlbHit++;
            }
            System.out.print(r.getId() + ", " +
                    physicalMemory.get(frameNbr).getEntry()[r.getOffsetVal()] + ", " +
                    frameNbr + ", ");
            print(physicalMemory.get(frameNbr).getEntry());
        }
    }

    private static int addToPhysicalMemory(byte[] data) {
        int frame;
        incrementPhysicalEntryCounts();

        if (physicalMemory.size() != frameNbrs) {
            physicalMemory.add(new PhysicalMemoryEntry(data));
            frame = physicalMemory.size() - 1;
        } else {
            frame = replacementAlgo(data);
        }
        return frame;
    }

    private static void incrementPhysicalEntryCounts() {
        for (PhysicalMemoryEntry p : physicalMemory) {
            p.incrementCount();
        }
    }

    private static int replacementAlgo(byte[] data) {
        int frame = 0;
        switch (replacement) {
            case FIFO:
                frame = fifo_lru(data);
                break;
            case LRU:
                frame = fifo_lru(data);
                break;
            case OPT:
                frame = opt(data);
                break;
        }
        return frame;
    }

    private static int opt(byte[] data) {
        int frame = 0, index;
        index = references.indexOf(opt);
        int i, count = 0;
        int lastVisited = -1;

        for (i = index; i < references.size(); i++) {
            if (count == frameNbrs) {
                break;
            }
            if (checkPageTable(references.get(i).getPageNbrVal()) != -1) {
                lastVisited = references.get(i).getPageNbrVal();
                if (!physicalMemory.get(pageTable[lastVisited].getFrameNbr()).isChecked()) {
                    count++;
                    physicalMemory.get(pageTable[lastVisited].getFrameNbr()).setChecked();

                }
            }
        }
        if (count < frameNbrs) {
            for (PhysicalMemoryEntry p : physicalMemory) {
                if (!p.isChecked()) {
                    frame = physicalMemory.indexOf(p);
                }
            }
        } else if (lastVisited != -1) {
            frame = pageTable[lastVisited].getFrameNbr();
        } else {
            frame = fifo_lru(data);
        }
        resetChecked();

        physicalMemory.set(frame, new PhysicalMemoryEntry(data));
        updatePageTable(frame);
        removeFromTLB(frame);

        return frame;
    }

    private static void resetChecked() {
        for (PhysicalMemoryEntry p : physicalMemory) {
            p.resetChecked();
        }
    }

    private static int fifo_lru(byte[] data) {
        int frame = 0, i;
        int firstIn = -1;
        PhysicalMemoryEntry p;

        for (i = 0; i < physicalMemory.size(); i++) {
            p = physicalMemory.get(i);
            if (firstIn < p.getCount()) {
                firstIn = p.getCount();
                frame = i;
            }
        }

        physicalMemory.set(frame, new PhysicalMemoryEntry(data));
        updatePageTable(frame);
        removeFromTLB(frame);

        return frame;
    }

    private static void updatePageTable(int frame) {
        for (PageEntry p : pageTable) {
            if (p != null && p.getFrameNbr() == frame) {
                p.deleteFrame();
            }
        }
    }

    private static void removeFromTLB(int frame) {
        TLBEntry toRemove = null;
        for (TLBEntry tlbEntry : tlb) {
            if (tlbEntry != null && tlbEntry.getFrameNbr() == frame) {
                toRemove = tlbEntry;
            }
        }

        tlb.remove(toRemove);
    }

    private static void updateTLB(int frameNbr, int pageNbr) {
        if (tlb.size() == 16) {
            //System.out.println("removed: " + tlb.poll().getPageNbr());
        } else {
            for (TLBEntry tlbEntry : tlb) {
                tlbEntry.incrementCount();
            }
            tlb.add(new TLBEntry(pageNbr, frameNbr));
        }
    }

    private static void addToPageTable(int frameNbr, int pageNbr) {
        pageTable[pageNbr] = new PageEntry(frameNbr);
    }

    private static void print(byte[] input) {
        for (byte b : input) {
            System.out.print(String.format("%02X", b));
        }
        System.out.println();
    }

    private static byte[] getFromFile(int pageNbr) {
        byte[] read = new byte[256];
        try {
            RandomAccessFile file = new RandomAccessFile("BACKING_STORE.bin", "r");
            file.seek(256 * pageNbr);
            file.read(read);
        } catch (Exception e) {
            System.out.println(e);
        }

        return read;
    }

    private static int checkTLB(int pageNbr) {
        for (TLBEntry e : tlb) {
            if (e != null && e.getPageNbr() == pageNbr) {
                return e.getFrameNbr();
            }
        }

        return -1;
    }

    private static int checkPageTable(int pageNbr) {
        if (pageTable[pageNbr] != null && pageTable[pageNbr].getValid()) {
            return pageTable[pageNbr].getFrameNbr();
        }

        return -1;
    }
}


class TLBEntry implements Comparable<TLBEntry> {
    private int pageNbr;
    private int frameNbr;
    private int count;

    TLBEntry(int pageNr, int frameNbr) {
        this.frameNbr = frameNbr;
        this.pageNbr = pageNr;
        this.count = 0;
    }

    public int getPageNbr() {
        return this.pageNbr;
    }

    public int getFrameNbr() {
        return this.frameNbr;
    }

    public void incrementCount() {
        this.count++;
    }

    @Override
    public int compareTo(TLBEntry o) {
        return o.count - this.count;
    }
}

class PageEntry {
    private boolean valid;
    private int frameNbr;

    PageEntry(int frameNbr) {
        this.valid = true;
        this.frameNbr = frameNbr;
    }

    public boolean getValid() {
        return this.valid;
    }

    public int getFrameNbr() {
        return this.frameNbr;
    }

    public void deleteFrame() {
        this.valid = false;
    }

}

class Reference {
    private byte pageNbr;
    private byte offset;
    private int pageNbrVal;
    private int offsetVal;
    private int id;

    Reference(byte pageNbr, byte offset, int id) {
        this.pageNbr = pageNbr;
        this.offset = offset;
        this.id = id;

        if ((offset & 0x80) == 0x80) {
            offset &= 0x7F;
            this.offsetVal = 128 + (offset);
        } else {
            this.offsetVal = offset;
        }
        if ((pageNbr & 0x80) == 0x80) {
            pageNbr &= 0x7F;
            this.pageNbrVal = 128 + (pageNbr);
        } else {
            this.pageNbrVal = pageNbr;
        }
    }

    public int getOffsetVal() {
        return offsetVal;
    }

    public int getPageNbrVal() {
        return this.pageNbrVal;
    }

    public int getId() {
        return id;
    }

    @Override
    public String toString() {
        return "id: " + id + " offset: " + String.format(" %X", offset) + " " + offsetVal + " page: " +
                String.format(" %X", pageNbr) + " " + pageNbrVal + "\n";
    }
}

class PhysicalMemoryEntry implements Comparable<PhysicalMemoryEntry> {
    private byte[] entry;
    private int count;
    private boolean checked;

    PhysicalMemoryEntry(byte[] read) {
        this.entry = read;
        this.count = 0;
        checked = false;
    }

    @Override
    public int compareTo(PhysicalMemoryEntry o) {
        return o.count - this.count;
    }

    public byte[] getEntry() {
        return entry;
    }

    public void incrementCount() {
        this.count++;
    }

    public int getCount() {
        return count;
    }

    public void resetCount() {
        this.count = 0;
    }

    public void setChecked() {
        this.checked = true;
    }

    public boolean isChecked() {
        return this.checked;
    }

    public void resetChecked() {
        this.checked = false;
    }
}
