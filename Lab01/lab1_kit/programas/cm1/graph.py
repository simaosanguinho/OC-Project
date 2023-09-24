import pandas as pd
import drawsvg as draw
import seaborn as sns
import matplotlib as mpl


df = pd.DataFrame()
df = pd.concat((df, pd.read_csv(f"/Users/simaosanguinho/Repos/OC-Project/Lab01/lab1_kit/programas/cm1/cm1.out", delimiter="\t")))


def strip_labels(val):
    value = val.split("=")[1]
    return float(value) if "." in value else int(value)


df = pd.DataFrame()
for i in range(10):
    df = pd.concat(
        (
            df,
            pd.read_csv(
                f"cm1.out",
                delimiter="\t",
                names=["cache_size", "stride", "avg_misses", "avg_time"],
                converters={
                    "cache_size": strip_labels,
                    "stride": strip_labels,
                    "avg_misses": strip_labels,
                    "avg_time": strip_labels,
                },
            ),
        )
    )

df = df.reset_index()

print(df)


df_avg = (
    df.groupby(["cache_size", "stride"], as_index=False).median().drop("index", axis=1)
)

df_avg.head()

mpl.rcParams["figure.figsize"] = [20, 12]
     
sns.set_theme(style='whitegrid', font_scale=1.5)

g_results = sns.lineplot(
    data=df_avg, x="stride", y="avg_misses", hue="cache_size", palette="Set2")
g_results.set_xscale("log", base=2)
g_results.set_xlabel("Strides (Bytes)")
g_results.set_ylabel("Average Misses")
g_results.get_legend().set_title("Cache Size (Bytes)")

mpl.pyplot.show()