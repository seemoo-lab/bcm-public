package de.tu_darmstadt.seemoo.nexmon;

import android.Manifest;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.content.res.AssetManager;
import android.net.Uri;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.CheckBox;
import android.widget.LinearLayout;
import android.widget.Spinner;
import android.widget.TextView;
import android.widget.Toast;

import com.stericson.RootShell.*;
import com.stericson.RootShell.exceptions.RootDeniedException;
import com.stericson.RootShell.execution.Command;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.concurrent.TimeoutException;

public class Nexmon extends AppCompatActivity {
    static {
        RootShell.debugMode = true;
    }

    private LinearLayout linDisclaimerDependent;
    private CheckBox chkDisclaimer;
    private Button btnAgreeToDisclaimer;
    private TextView tvInstallUtilities;
    private CheckBox chkNexutil;
    private CheckBox chkDhdutil;
    private CheckBox chkTcpdump;
    private CheckBox chkAircrack;
    private CheckBox chkLibfakeioctl;
    private CheckBox chkNetcat;
    private CheckBox chkIw;
    private CheckBox chkWirelessTools;
    private CheckBox chkMdk3;
    private Spinner spnBinInstallLocation;
    private Spinner spnLibInstallLocation;
    private TextView tvFirmwareVersionOutput;
    private Button btnCreateFirmwareBackup;
    private Button btnRestoreFirmwareBackup;
    private Button btnInstallNexmonFirmware;
    private TextView tvDhdutilDumpConsoleOutput;
    private TextView tvNexutilGetMonitorStatusOutput;
    private TextView tvTcpdumpOnlyOutput;
    private TextView tvTcpdumpWithLibfakeioctlOutput;
    private TextView tvTcpdumpWithLibfakeioctlWithoutRadiotapOutput;
    private TextView tvAircrackFrameInjectionOutput;

    private boolean rootAvailable = false;
    private boolean rootAccess = false;

    private boolean disclaimerButtonState = false;

    private void toast(String msg) {
        Toast.makeText(getApplicationContext(), msg, Toast.LENGTH_SHORT).show();
    }

    private void toastLogcatError() {
        toast("Sorry, there was an error, find out more about it using logcat");
    }

    private void getGuiElements() {
        linDisclaimerDependent = (LinearLayout) findViewById(R.id.linDisclaimerDependent);
        chkDisclaimer = (CheckBox) findViewById(R.id.chkDisclaimer);
        btnAgreeToDisclaimer = (Button) findViewById(R.id.btnAgreeToDisclaimer);
        tvInstallUtilities = (TextView) findViewById(R.id.tvInstallUtilities);
        chkNexutil = (CheckBox) findViewById(R.id.chkNexutil);
        chkDhdutil = (CheckBox) findViewById(R.id.chkDhdutil);
        chkTcpdump = (CheckBox) findViewById(R.id.chkTcpdump);
        chkAircrack = (CheckBox) findViewById(R.id.chkAircrack);
        chkLibfakeioctl = (CheckBox) findViewById(R.id.chkLibfakeioctl);
        chkNetcat = (CheckBox) findViewById(R.id.chkNetcat);
        chkIw = (CheckBox) findViewById(R.id.chkIw);
        chkWirelessTools = (CheckBox) findViewById(R.id.chkWirelessTools);
        chkMdk3 = (CheckBox) findViewById(R.id.chkMdk3);
        spnBinInstallLocation = (Spinner) findViewById(R.id.spnBinInstallLocation);
        spnLibInstallLocation = (Spinner) findViewById(R.id.spnLibInstallLocation);
        tvFirmwareVersionOutput = (TextView) findViewById(R.id.tvFirmwareVersionOutput);
        btnCreateFirmwareBackup = (Button) findViewById(R.id.btnCreateFirmwareBackup);
        btnRestoreFirmwareBackup = (Button) findViewById(R.id.btnRestoreFirmwareBackup);
        btnInstallNexmonFirmware = (Button) findViewById(R.id.btnInstallNexmonFirmware);
        tvDhdutilDumpConsoleOutput = (TextView) findViewById(R.id.tvDhdutilDumpConsoleOutput);
        tvNexutilGetMonitorStatusOutput = (TextView) findViewById(R.id.tvNexutilGetMonitorStatusOutput);
        tvTcpdumpOnlyOutput = (TextView) findViewById(R.id.tvTcpdumpOnlyOutput);
        tvTcpdumpWithLibfakeioctlOutput = (TextView) findViewById(R.id.tvTcpdumpWithLibfakeioctlOutput);
        tvTcpdumpWithLibfakeioctlWithoutRadiotapOutput = (TextView) findViewById(R.id.tvTcpdumpWithLibfakeioctlWithoutRadiotapOutput);
        tvAircrackFrameInjectionOutput = (TextView) findViewById(R.id.tvAircrackFrameInjectionOutput);
    }

    private void initializeInstallLocationSpinners() {
        ArrayAdapter<CharSequence> adapter = ArrayAdapter.createFromResource(this,
                R.array.binInstallLocations, android.R.layout.simple_spinner_item);
        adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        spnBinInstallLocation.setAdapter(adapter);

        adapter = ArrayAdapter.createFromResource(this,
                R.array.libInstallLocations, android.R.layout.simple_spinner_item);
        adapter.setDropDownViewResource(android.R.layout.simple_spinner_dropdown_item);
        spnLibInstallLocation.setAdapter(adapter);
    }

    private void requestPermission(String permission) {
        if (ContextCompat.checkSelfPermission(this, permission) != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(this, new String[]{permission}, 0);
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_nexmon);

        getGuiElements();
        initializeInstallLocationSpinners();

        linDisclaimerDependent.setVisibility(LinearLayout.GONE);

        requestPermission(Manifest.permission.WRITE_EXTERNAL_STORAGE);
        requestPermission(Manifest.permission.READ_EXTERNAL_STORAGE);

        if ((new File("/sdcard/fw_bcmdhd.orig.bin")).exists()) {
            btnCreateFirmwareBackup.setEnabled(false);
        } else {
            btnRestoreFirmwareBackup.setEnabled(false);
            btnInstallNexmonFirmware.setEnabled(false);
        }
    }

    private void copyFile(InputStream in, OutputStream out) throws IOException {
        byte[] buffer = new byte[1024];
        while (in.read(buffer) != -1) {
            out.write(buffer);
        }
    }

    private void extractAssets() throws IOException {
        AssetManager assetManager = getAssets();
        String[] files = null;
        files = assetManager.list("nexmon");
        if (files != null) for (String filename : files) {
            InputStream in = null;
            OutputStream out = null;
            in = assetManager.open("nexmon/" + filename);
            File outFile = new File(getExternalFilesDir(null), filename);
            out = new FileOutputStream(outFile);
            copyFile(in, out);
            if (in != null) in.close();
            if (out != null) out.close();
        }
    }

    public void onClickAgreeToDisclaimer(View v) {
        if (chkDisclaimer.isChecked()) {
            if (RootShell.isRootAvailable()) {
                rootAvailable = true;
                if (RootShell.isAccessGiven()) {
                    rootAccess = true;

                    if (disclaimerButtonState) {
                        linDisclaimerDependent.setVisibility(LinearLayout.GONE);
                        btnAgreeToDisclaimer.setText("I solemnly swear that I am up to no good");
                    } else {
                        linDisclaimerDependent.setVisibility(LinearLayout.VISIBLE);
                        btnAgreeToDisclaimer.setText("Mischief managed!");
                    }
                    disclaimerButtonState = !disclaimerButtonState;
                } else {
                    Toast.makeText(getApplicationContext(), "Sorry, but to install and run the required tools, we require root access to your device.", Toast.LENGTH_SHORT).show();
                }
            } else {
                Toast.makeText(getApplicationContext(), "Sorry, we did not detect a su binary on your device. Please, root your device before continuing.", Toast.LENGTH_SHORT).show();
            }
        } else {
            Toast.makeText(getApplicationContext(), "Sorry, but you have to take more responsibility.", Toast.LENGTH_SHORT).show();
        }
    }

    private void copyExtractedAsset(String installLocation, String filename) throws TimeoutException, RootDeniedException, IOException {
        RootShell.getShell(true).add(new Command(0, "mount -o remount,rw /system",
                "cp " + getExternalFilesDir(null) + "/" + filename + " " + installLocation,
                "chmod 755 " + installLocation + "/" + filename) {
            @Override
            public void commandOutput(int id, String line) {
                Toast.makeText(getApplicationContext(), line, Toast.LENGTH_SHORT).show();

                //MUST call the super method when overriding!
                super.commandOutput(id, line);
            }
        });
    }

    public void onClickInstall(View v) {
        String binInstallLocation = spnBinInstallLocation.getSelectedItem().toString();
        String libInstallLocation = spnLibInstallLocation.getSelectedItem().toString();

        try {
            extractAssets();

            if (chkDhdutil.isChecked()) {
                toast("Installing dhdutil ...");
                copyExtractedAsset(binInstallLocation, "dhdutil");
            }

            if (chkNexutil.isChecked()) {
                toast("Installing nexutil ...");
                copyExtractedAsset(binInstallLocation, "nexutil");
            }

            if (chkTcpdump.isChecked()) {
                toast("Installing tcpdump ...");
                copyExtractedAsset(binInstallLocation, "tcpdump");
            }

            if (chkLibfakeioctl.isChecked()) {
                toast("Installing tcpdump ...");
                copyExtractedAsset(libInstallLocation, "libfakeioctl.so");
            }

            if (chkAircrack.isChecked()) {
                toast("Installing aircrack-ng suite ...");
                copyExtractedAsset(binInstallLocation, "airbase-ng");
                copyExtractedAsset(binInstallLocation, "aircrack-ng");
                copyExtractedAsset(binInstallLocation, "airdecap-ng");
                copyExtractedAsset(binInstallLocation, "airdecloak-ng");
                copyExtractedAsset(binInstallLocation, "aireplay-ng");
                copyExtractedAsset(binInstallLocation, "airodump-ng");
                copyExtractedAsset(binInstallLocation, "airolib-ng");
                copyExtractedAsset(binInstallLocation, "airserv-ng");
                copyExtractedAsset(binInstallLocation, "airtun-ng");
                copyExtractedAsset(binInstallLocation, "besside-ng");
                copyExtractedAsset(binInstallLocation, "buddy-ng");
                copyExtractedAsset(binInstallLocation, "easside-ng");
                copyExtractedAsset(binInstallLocation, "ivstools");
                copyExtractedAsset(binInstallLocation, "kstats");
                copyExtractedAsset(binInstallLocation, "makeivs-ng");
                copyExtractedAsset(binInstallLocation, "packetforge-ng");
                copyExtractedAsset(binInstallLocation, "tkiptun-ng");
                copyExtractedAsset(binInstallLocation, "wesside-ng");
                copyExtractedAsset(binInstallLocation, "wpaclean");
            }

            if (chkNetcat.isChecked()) {
                toast("Installing netcat ...");
                copyExtractedAsset(binInstallLocation, "nc");
            }

            if (chkIw.isChecked()) {
                toast("Installing iw ...");
                copyExtractedAsset(binInstallLocation, "iw");
            }

            if (chkWirelessTools.isChecked()) {
                toast("Installing wireless-tools ...");
                copyExtractedAsset(binInstallLocation, "iwconfig");
                copyExtractedAsset(binInstallLocation, "iwlist");
                copyExtractedAsset(binInstallLocation, "iwpriv");
            }

            if (chkMdk3.isChecked()) {
                toast("Installing mdk3 ...");
                copyExtractedAsset(binInstallLocation, "mdk3");
            }

        } catch (Exception e) {
            e.printStackTrace();
            toastLogcatError();
        }
    }

    public void onClickPrintFirmwareVersion(View v) {
        Command command = new Command(0, "strings /vendor/firmware/fw_bcmdhd.bin | grep \"FWID:\"") {
            @Override
            public void commandOutput(int id, String line) {
                tvFirmwareVersionOutput.append(line + "\n");

                //MUST call the super method when overriding!
                super.commandOutput(id, line);
            }
        };
        try {
            RootShell.getShell(true).add(command);
        } catch (Exception e) {
            e.printStackTrace();
            toastLogcatError();
        }
    }

    public void onClickPrintFirmwareVersionClear(View v) {
        tvFirmwareVersionOutput.setText("");
    }

    public void onClickCreateFirmwareBackup(View v) {
        Command command = new Command(0, "cp /vendor/firmware/fw_bcmdhd.bin /sdcard/fw_bcmdhd.orig.bin") {
            @Override
            public void commandOutput(int id, String line) {
                Toast.makeText(getApplicationContext(), line, Toast.LENGTH_SHORT).show();

                //MUST call the super method when overriding!
                super.commandOutput(id, line);
            }
        };
        try {
            RootShell.getShell(true).add(command);
        } catch (Exception e) {
            e.printStackTrace();
            toastLogcatError();
        }

        btnCreateFirmwareBackup.setEnabled(false);
        btnRestoreFirmwareBackup.setEnabled(true);
        btnInstallNexmonFirmware.setEnabled(true);
    }

    public void onClickRestoreFirmwareBackup(View v) {
        Command command = new Command(0, "mount -o remount,rw /system", "cp /sdcard/fw_bcmdhd.orig.bin /vendor/firmware/fw_bcmdhd.bin") {
            @Override
            public void commandOutput(int id, String line) {
                Toast.makeText(getApplicationContext(), line, Toast.LENGTH_SHORT).show();

                //MUST call the super method when overriding!
                super.commandOutput(id, line);
            }
        };
        try {
            RootShell.getShell(true).add(command);
        } catch (Exception e) {
            e.printStackTrace();
            toastLogcatError();
        }
    }

    public void onClickInstallNexmonFirmware(View v) {
        try {
            extractAssets();
            toast("Installing fw_bcmdhd.bin ...");
            copyExtractedAsset("/vendor/firmware/", "fw_bcmdhd.bin");
        } catch (Exception e) {
            e.printStackTrace();
            toastLogcatError();
        }
    }

    public void onClickDhdutilDumpConsole(View v) {
        try {
            Command command = new Command(0, "dhdutil consoledump") {
                @Override
                public void commandOutput(int id, String line) {
                    tvDhdutilDumpConsoleOutput.append(line + "\n");

                    //MUST call the super method when overriding!
                    super.commandOutput(id, line);
                }
            };
            RootShell.getShell(true).add(command);
        } catch (Exception e) {
            e.printStackTrace();
            toastLogcatError();
        }
    }

    public void onClickDhdutilDumpConsoleClear(View v) {
        tvDhdutilDumpConsoleOutput.setText("");
    }

    public void onClickNexutilActivateMonitor(View v) {
        try {
            Command command = new Command(0, "nexutil --set-monitor true") {
                @Override
                public void commandOutput(int id, String line) {
                    Toast.makeText(getApplicationContext(), line, Toast.LENGTH_SHORT).show();

                    //MUST call the super method when overriding!
                    super.commandOutput(id, line);
                }
            };
            RootShell.getShell(true).add(command);
        } catch (Exception e) {
            e.printStackTrace();
            toastLogcatError();
        }
    }

    public void onClickNexutilDeactivateMonitor(View v) {
        try {
            Command command = new Command(0, "nexutil --set-monitor false") {
                @Override
                public void commandOutput(int id, String line) {
                    Toast.makeText(getApplicationContext(), line, Toast.LENGTH_SHORT).show();

                    //MUST call the super method when overriding!
                    super.commandOutput(id, line);
                }
            };
            RootShell.getShell(true).add(command);
        } catch (Exception e) {
            e.printStackTrace();
            toastLogcatError();
        }
    }

    public void onClickNexutilGetMonitorStatus(View v) {
        try {
            Command command = new Command(0, "nexutil --get-monitor") {
                @Override
                public void commandOutput(int id, String line) {
                    tvNexutilGetMonitorStatusOutput.append(line + "\n");

                    //MUST call the super method when overriding!
                    super.commandOutput(id, line);
                }
            };
            RootShell.getShell(true).add(command);
        } catch (Exception e) {
            e.printStackTrace();
            toastLogcatError();
        }
    }

    public void onClickNexutilGetMonitorStatusClear(View v) {
        tvNexutilGetMonitorStatusOutput.setText("");
    }

    public void onClickTcpdumpOnly(View v) {
        try {
            Command command = new Command(0, "tcpdump -i wlan0 -c 10 & sleep 5; kill $!") {
                @Override
                public void commandOutput(int id, String line) {
                    tvTcpdumpOnlyOutput.append(line + "\n");

                    //MUST call the super method when overriding!
                    super.commandOutput(id, line);
                }
            };
            RootShell.getShell(true).add(command);
        } catch (Exception e) {
            e.printStackTrace();
            toastLogcatError();
        }
    }

    public void onClickTcpdumpOnlyClear(View v) {
        tvTcpdumpOnlyOutput.setText("");
    }

    public void onClickTcpdumpWithLibfakeioctl(View v) {
        try {
            Command command = new Command(0, "LD_PRELOAD=libfakeioctl.so tcpdump -i wlan0 -c 10 & sleep 5; kill $!") {
                @Override
                public void commandOutput(int id, String line) {
                    tvTcpdumpWithLibfakeioctlOutput.append(line + "\n");

                    //MUST call the super method when overriding!
                    super.commandOutput(id, line);
                }
            };
            RootShell.getShell(true).add(command);
        } catch (Exception e) {
            e.printStackTrace();
            toastLogcatError();
        }
    }

    public void onClickTcpdumpWithLibfakeioctlClear(View v) {
        tvTcpdumpWithLibfakeioctlOutput.setText("");
    }

    public void onClickTcpdumpWithLibfakeioctlWithoutRadiotap(View v) {
        try {
            Command command = new Command(0, "LD_PRELOAD=libfakeioctl.so NEXMON_SA_FAMILY=ARPHRD_IEEE80211 tcpdump -i wlan0 -c 10 & sleep 5; kill $!") {
                @Override
                public void commandOutput(int id, String line) {
                    tvTcpdumpWithLibfakeioctlWithoutRadiotapOutput.append(line + "\n");

                    //MUST call the super method when overriding!
                    super.commandOutput(id, line);
                }
            };
            RootShell.getShell(true).add(command);
        } catch (Exception e) {
            e.printStackTrace();
            toastLogcatError();
        }
    }

    public void onClickTcpdumpWithLibfakeioctlWithoutRadiotapClear(View v) {
        tvTcpdumpWithLibfakeioctlWithoutRadiotapOutput.setText("");
    }

    public void onClickAircrackFrameInjection(View v) {
        try {
            Command command = new Command(0, "LD_PRELOAD=libfakeioctl.so aireplay-ng --test wlan0") {
                @Override
                public void commandOutput(int id, String line) {
                    tvAircrackFrameInjectionOutput.append(line + "\n");

                    //MUST call the super method when overriding!
                    super.commandOutput(id, line);
                }
            };
            RootShell.getShell(true).add(command);
        } catch (Exception e) {
            e.printStackTrace();
            toastLogcatError();
        }
    }

    public void onClickAircrackFrameInjectionClear(View v) {
        tvAircrackFrameInjectionOutput.setText("");
    }

    public void onClickNexmon(View v) {
        Intent intent = new Intent();
        intent.setAction(Intent.ACTION_VIEW);
        intent.addCategory(Intent.CATEGORY_BROWSABLE);
        intent.setData(Uri.parse("https://nexmon.org"));
        startActivity(intent);
    }

    public void onClickSeemoo(View v) {
        Intent intent = new Intent();
        intent.setAction(Intent.ACTION_VIEW);
        intent.addCategory(Intent.CATEGORY_BROWSABLE);
        intent.setData(Uri.parse("https://seemoo.tu-darmstadt.de"));
        startActivity(intent);
    }

    public void onClickTuDarmstadt(View v) {
        Intent intent = new Intent();
        intent.setAction(Intent.ACTION_VIEW);
        intent.addCategory(Intent.CATEGORY_BROWSABLE);
        intent.setData(Uri.parse("https://www.tu-darmstadt.de"));
        startActivity(intent);
    }
}
